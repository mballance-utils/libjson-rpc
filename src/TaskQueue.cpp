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


TaskQueue::TaskQueue(jrpc::IEventLoop *loop) : m_loop(loop) {

}

TaskQueue::~TaskQueue() {

}

void TaskQueue::run() {
    bool ret = m_queue.front()->run(this);
    if (ret) {
        ITaskUP task(m_queue.front().release());
        m_queue.erase(m_queue.begin());
        addTask(task);
    } else {
        m_queue.erase(m_queue.begin());

        if (m_queue.size()) {
            m_loop->addIdleTask([this]() { this->run(); });
        }
    }
}

void TaskQueue::addTask(ITaskUP &task) {
    m_queue.push_back(std::move(task));

    if (m_queue.size() == 1) {
        m_loop->addIdleTask([this]() { this->run(); });
    }
}

}
