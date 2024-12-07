/*
 * TaskQueue.cpp
 *
 * Copyright 2023 Matthew Ballance and Contributors
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
#include "dmgr/impl/DebugMacros.h"
#include "TaskQueue.h"


namespace jrpc {


TaskQueue::TaskQueue(dmgr::IDebugMgr *dmgr, ITaskScheduler *sched) : 
    m_scheduler(sched), m_idle_scheduled(false), m_pending(0),
    m_closing(false) {
    DEBUG_INIT("jrpc::TaskQueue", dmgr);
}

TaskQueue::~TaskQueue() {

}

bool TaskQueue::runOneTask() {
    ITask *next = 0;

    m_mutex.lock();
    if (m_queue.size()) {
        next = m_queue.front().first;
        m_queue.erase(m_queue.begin());
    }
    m_mutex.unlock();

    if (next) {
        next->clrFlags(TaskFlags::Yield);
        next->run(0, false);
    }

    return next;
}

bool TaskQueue::runOneWorkerTask() {
    bool ret = true;

    while (true) {
        TaskE task = {0, false};
        m_mutex.lock();
        if (m_queue.size()) {
            task = m_queue.front();
            m_queue.erase(m_queue.begin());
        }
        m_pending++;
        m_mutex.unlock();

        if (task.first) {
            m_mutex.lock();
            m_pending--;
            m_mutex.unlock();
        } else {
            // Need to wait for something to happen
        }
    }
    // Assumed to have 

    return ret;
}

void TaskQueue::run() {
    m_idle_scheduled = false;
    fprintf(stderr, "TaskQueue::run size=%d\n", m_queue.size());
    if (m_queue.size() == 0) {
        fprintf(stderr, "ERROR: zero-size queue\n");
        return;
    }
    TaskE task = m_queue.front();
    m_queue.erase(m_queue.begin());

    ITask *next = task.first->run(0, false);

/*
    if (status == TaskStatus::Yield) {
        addTask(task.first, task.second);
    } else if (status == TaskStatus::Done && task.second) {
        delete task.first;
    }
 */

    /*
    if (ret) {
        ITaskUP task(m_queue.front().release());
        m_queue.erase(m_queue.begin());
        addTask(task);
    } else {
        m_queue.erase(m_queue.begin());
    */

#ifdef UNDEFINED
    if (m_loop && m_queue.size() > 0 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
#endif
    fprintf(stderr, "<TaskQueue::run size=%d\n", m_queue.size());

//    }
}

bool TaskQueue::havePending() {
    bool ret;
    m_mutex.lock();
    ret = m_queue.size();
    m_mutex.unlock();

    return ret;
}

void TaskQueue::addTask(ITask *task, bool owned) {
    m_mutex.lock();

    if (task->hasFlags(TaskFlags::Queued)) {
        DEBUG("Note: task is already queued");
        m_mutex.unlock();
        return;
    } else {
        // This is really debug code...
        for (std::vector<TaskE>::const_iterator
            it=m_queue.begin();
            it!=m_queue.end(); it++) {
            if (it->first == task) {
                DEBUG_ERROR("Task already in queue");
                m_mutex.unlock();
                return;
            }
        }
    }

    task->setFlags(TaskFlags::Queued);
    m_queue.push_back({task, owned});

#ifdef UNDEFINED
    if (m_loop && m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
#endif
    m_mutex.unlock();
}

void TaskQueue::queueTask(ITask *task) {
    m_mutex.lock();
    task->setFlags(TaskFlags::Queued);
    m_queue.push_back({task, false});

#ifdef UNDEFINED
    if (m_loop && m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
#endif
    m_mutex.unlock();
}

void TaskQueue::scheduleTask(ITask *task, uint64_t n_us) {
    m_scheduler->scheduleTask(task, n_us);
}

dmgr::IDebug *TaskQueue::m_dbg = 0;

}
