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
#include "EventLoop.h"


namespace jrpc {


EventLoop::EventLoop(dmgr::IDebugMgr *dmgr) {
    DEBUG_INIT("EventLoop", dmgr);
}

EventLoop::~EventLoop() {

}

int32_t EventLoop::process_one_event(int32_t timeout_ms) {
    int32_t ret = 0;

    DEBUG_ENTER("process_one_event");

    // Setup masks for 
    if (m_read_tasks.size() || m_write_tasks.size()) {
        int32_t max_fd = -1;
        fd_set      read_s, write_s, except_s;
        struct timeval timeout;
        struct timeval *timeout_p = &timeout;

        if (timeout_ms > 0) {
            timeout.tv_sec = timeout_ms/1000;
            timeout.tv_usec = (timeout_ms%1000) * 1000;
        } else if (timeout_ms < 0) {
            timeout_p = 0;
        } else {
            // Zero wait
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
        }

        FD_ZERO(&read_s);
        FD_ZERO(&write_s);
        FD_ZERO(&except_s);

        std::vector<FdTask> read_tasks(
            m_read_tasks.begin(),
            m_read_tasks.end());
        m_read_tasks.clear();
        std::vector<FdTask> write_tasks(
            m_write_tasks.begin(),
            m_write_tasks.end());
        m_write_tasks.clear();

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

        int32_t res = ::select(
            max_fd+1, &read_s, &write_s, &except_s, timeout_p);

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
    }

    if (!ret && m_idle_tasks.size()) {
        m_idle_tasks.at(0)();
        m_idle_tasks.erase(m_idle_tasks.begin());
    }

    DEBUG_LEAVE("process_one_event %d", ret);
    return ret;
}

void EventLoop::addIdleTask(std::function<void ()> task) {
    m_idle_tasks.push_back(task);
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

dmgr::IDebug *EventLoop::m_dbg = 0;

}
