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

bool TaskQueue::runOneTask() {
    ITask *next = 0;

    m_mutex.lock();
    if (m_queue.size()) {
        next = m_queue.front().first;
        m_queue.erase(m_queue.begin());
    }
    m_mutex.unlock();

    if (next) {
        next->run(0, false);
    }

    return next;
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

    if (m_queue.size() > 0 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    fprintf(stderr, "<TaskQueue::run size=%d\n", m_queue.size());

//    }
}

void TaskQueue::addTask(ITask *task, bool owned) {
    fprintf(stderr, "TaskQueue::addTask size=%d\n", m_queue.size());

    m_mutex.lock();
    task->setFlags(TaskFlags::Queued);
    m_queue.push_back({task, owned});

    if (m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    m_mutex.unlock();

    fprintf(stderr, "<TaskQueue::addTask size=%d\n", m_queue.size());
}

void TaskQueue::addTaskPreempt(ITask *task, bool owned) {
    fprintf(stderr, "TaskQueue::addTaskPreempt size=%d\n", m_queue.size());
    m_queue.insert(m_queue.begin(), {task, owned});

    if (m_queue.size() == 1 && !m_idle_scheduled) {
        fprintf(stderr, "  Schedule idle\n");
        m_loop->addIdleTask([this]() { this->run(); });
        m_idle_scheduled = true;
    }
    fprintf(stderr, "<TaskQueue::addTaskPreempt size=%d\n", m_queue.size());
}

}
