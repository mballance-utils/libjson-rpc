/*
 * TaskGroup.cpp
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
#include "TaskGroup.h"


namespace jrpc {


TaskGroup::TaskGroup(ITaskQueue *queue) : m_queue(queue) {

}

TaskGroup::~TaskGroup() {

}

void TaskGroup::pushTask(ITask *t) {
    m_task_s.push_back({t, false});
}

void TaskGroup::suspTask() {
    if (!m_task_s.size() || m_task_s.back().second) {
        // ERROR
    }

    ITask *tc = m_task_s.back().first->clone();
    m_task_s.back() = {tc, true};
}

void TaskGroup::wakeTask() {
    // TODO: clear flags?

    // Schedule the task-group to run
    m_queue->addTask(this, true);
}

void TaskGroup::popTask() {
    TaskE t = m_task_s.back();
    m_task_s.pop_back();

    if (m_task_s.size()) {
        // Propagate result and flags
//        TaskResult r = t.first->moveResult();
//        m_task_s.back().first->setResult(r);
    } else {
        // 
        m_result = t.first->moveResult();
    }

    if (t.second) {
        delete t.first;
    }
}

TaskStatus TaskGroup::run() {
    TaskStatus ret;

    while (m_task_s.size()) {
        if ((ret=m_task_s.back().first->run()) == TaskStatus::Done) {
            popTask();
        } else {
            break;
        }
    }

    return ret;
}

bool TaskGroup::hasFlags(TaskFlags flags) {
    
}

void TaskGroup::setFlags(TaskFlags flags) {

}

void TaskGroup::clrFlags(TaskFlags flags) {

}

void TaskGroup::setResult(const TaskResult &r) {

}

void TaskGroup::setResult(TaskResult &r) {

}

const TaskResult &TaskGroup::getResult() const {
    return m_result;
}

TaskResult &TaskGroup::moveResult() {
    return m_result;
}

}
