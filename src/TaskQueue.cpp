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
#include "TaskQueue.h"


namespace jrpc {


TaskQueue::TaskQueue(jrpc::IEventLoop *loop) : 
    m_idle_scheduled(false), m_loop(loop) {

}

TaskQueue::~TaskQueue() {

}

void TaskQueue::run() {
    m_idle_scheduled = false;
    fprintf(stderr, "TaskQueue::run size=%d\n", m_queue.size());
    if (m_queue.size() == 0) {
        fprintf(stderr, "ERROR: zero-size queue\n");
        return;
    }
    ITaskUP task(std::move(m_queue.front()));
    m_queue.erase(m_queue.begin());
    bool ret = task->run(this);

    /*
    if (ret) {
        ITaskUP task(m_queue.front().release());
        m_queue.erase(m_queue.begin());
        addTask(task);
    } else {
        m_queue.erase(m_queue.begin());
    */

    if (m_queue.size() > 0 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    fprintf(stderr, "<TaskQueue::run size=%d\n", m_queue.size());

//    }
}

void TaskQueue::addTask(ITaskUP &task) {
    fprintf(stderr, "TaskQueue::addTask size=%d\n", m_queue.size());
    m_queue.push_back(std::move(task));

    if (m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    fprintf(stderr, "<TaskQueue::addTask size=%d\n", m_queue.size());
}

void TaskQueue::addTaskPreempt(ITaskUP &task) {
    fprintf(stderr, "TaskQueue::addTaskPreempt size=%d\n", m_queue.size());
    m_queue.insert(
        m_queue.begin(),
        std::move(task));

    if (m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    fprintf(stderr, "<TaskQueue::addTaskPreempt size=%d\n", m_queue.size());
}

}
