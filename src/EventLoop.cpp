/*
 * EventLoop.cpp
 *
 * Copyright 2022 Matthew Ballance and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may 
 * not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * Created on:
 *     Author:
 */
#include <sys/select.h>
#include "dmgr/impl/DebugMacros.h"
#include "jrpc/ITaskQueue.h"
#include "EventLoop.h"
#include <sys/poll.h>
#include <time.h>


namespace jrpc {


EventLoop::EventLoop(dmgr::IDebugMgr *dmgr) {
    DEBUG_INIT("EventLoop", dmgr);
}

EventLoop::~EventLoop() {

}

int32_t EventLoop::process_one_event(int32_t timeout_ms) {
    int32_t ret = 0;
    struct timeval timeout;

    DEBUG_ENTER("process_one_event (%d) read_tasks=%d write_tasks=%d",
        timeout_ms, m_read_tasks.size(), m_write_tasks.size());

    // First, determine how much time to wait
    uint64_t time_q_wait_us = 0;
    uint64_t api_req_us = timeout_ms * 1000;

    if (m_task_q && m_task_q->havePending()) {
        time_q_wait_us = 0;
    } else if (m_time_q.size()) {
        time_q_wait_us = m_time_q.at(0).first;
    }

    if (time_q_wait_us < api_req_us) {
        timeout.tv_sec = time_q_wait_us / (1000*1000);
        timeout.tv_usec = time_q_wait_us % (1000*1000);
    } else {
        timeout.tv_sec = api_req_us / (1000*1000);
        timeout.tv_usec = api_req_us % (1000*1000);
    }

    // Setup masks for 
    if (m_read_tasks.size() || m_write_tasks.size()) {

        int32_t max_fd = -1;
        fd_set      read_s, write_s, except_s;
        struct timeval *timeout_p = &timeout;

        std::vector<FdTask> write_tasks(
            m_write_tasks.begin(),
            m_write_tasks.end());
        m_write_tasks.clear();

        std::vector<FdTask> read_tasks(
            m_read_tasks.begin(),
            m_read_tasks.end());
        m_read_tasks.clear();

        FD_ZERO(&read_s);
        FD_ZERO(&write_s);
        FD_ZERO(&except_s);

        for (std::vector<FdTask>::const_iterator
            it=read_tasks.begin();
            it!=read_tasks.end(); it++) {
            if (it->first > max_fd) {
                max_fd = it->first;
            }
            FD_SET(it->first, &read_s);
        }

        for (std::vector<FdTask>::const_iterator
            it=write_tasks.begin();
            it!=write_tasks.end(); it++) {
            if (it->first > max_fd) {
                max_fd = it->first;
            }
            FD_SET(it->first, &write_s);
        }

        DEBUG_ENTER("select: max_fd=%d timeout_p=%p", max_fd+1, timeout_p);
        int32_t res = ::select(
            max_fd+1, &read_s, &write_s, &except_s, timeout_p);
        DEBUG_LEAVE("select: res=%d", res);

        if (res > 0) {
            // Process each event
            for (std::vector<FdTask>::const_iterator
                it=read_tasks.begin();
                it!=read_tasks.end(); it++) {
                if (FD_ISSET(it->first, &read_s)) {
                    it->second();
                } else {
                    m_read_tasks.push_back(*it);
                }
            }

            for (std::vector<FdTask>::const_iterator
                it=write_tasks.begin();
                it!=write_tasks.end(); it++) {
                if (FD_ISSET(it->first, &write_s)) {
                    it->second();
                } else {
                    m_write_tasks.push_back(*it);
                }
            }
            ret = 1;
        } else {
            // Just re-insert all the tasks
            m_read_tasks.insert(
                m_read_tasks.end(),
                read_tasks.begin(),
                read_tasks.end());
            m_write_tasks.insert(
                m_write_tasks.end(),
                write_tasks.begin(),
                write_tasks.end());
        }
    } else {
        // No sockets. What about time?
        timespec ts, ts_r;
        ts.tv_sec = timeout.tv_sec;
        ts.tv_nsec = (timeout.tv_usec * 1000);

        nanosleep(&ts, &ts_r);
    }

    // Now, update the time queue
    m_mutex.lock();
    if (m_time_q.size()) {
        uint64_t time_us = timeout.tv_sec;
        time_us *= (1000ULL*1000ULL);
        time_us += timeout.tv_usec;

        if (m_time_q.at(0).first > time_us) {
            m_time_q.at(0).first -= time_us;
        } else {
            m_time_q.at(0).first = 0;
        }
    }
    m_mutex.unlock();

    while (true) {
        ITask *task = 0;
        m_mutex.lock();
        if (m_time_q.size() && !m_time_q.at(0).first) {
            task = m_time_q.at(0).second;
            m_time_q.erase(m_time_q.begin());
        }
        m_mutex.unlock();

        if (task) {
            task->queue();
        } else {
            break;
        }
    }

    if (m_task_q) {
        m_task_q->runOneTask();
    }

    DEBUG_LEAVE("process_one_event %d", ret);
    return ret;
}

void EventLoop::addAfterTask(
        std::function<void ()>  task,
        int32_t                 ms) {
    m_timed_tasks.push_back({ms, task});
}

void EventLoop::addFdReadTask(
        std::function<void ()>  task,
        int32_t                 fd) {
    m_read_tasks.push_back({fd, task});
}

void EventLoop::addFdWriteTask(
        std::function<void ()>  task,
        int32_t                 fd) {
    m_write_tasks.push_back({fd, task});
}

void EventLoop::scheduleTask(ITask *task, uint64_t n_us) {
    uint32_t i=0; 

    m_mutex.lock();
    for (; i<m_time_q.size(); i++) {
        if (n_us < m_time_q.at(i).first) {
            m_time_q.insert(m_time_q.begin()+i, {n_us, task});
            break;
        } else {
            n_us -= m_time_q.at(i).first;
        }
    }

    if (i == m_time_q.size()) {
        m_time_q.push_back({n_us, task});
    }

    m_mutex.unlock();
}

void EventLoop::cancelSchedule(ITask *task) {

}

dmgr::IDebug *EventLoop::m_dbg = 0;

}
