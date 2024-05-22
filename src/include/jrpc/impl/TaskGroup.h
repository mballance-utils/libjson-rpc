/**
 * TaskGroup.h
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
#pragma once
#include <vector>
#include "jrpc/ITaskGroup.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {


class TaskGroup : public TaskBase {
public:
    TaskGroup(ITaskQueue *queue) : TaskBase(queue) { }

    TaskGroup(TaskGroup *o) : TaskBase(o) { }

    virtual ~TaskGroup() { }

    virtual ITask *run(ITask *parent, bool initial) override {
        runEnter(parent, initial);

        if (initial) {
            runStart();
        }
        // Check whether we are complete
        bool done;
        m_mutex.lock();
        done = (m_subtasks.size() == 0);
        if (!done) {
            for (std::vector<ITask *>::const_iterator
                it=m_subtasks.begin();
                it!=m_subtasks.end(); it++) {
                (*it)->addCompletionMon(std::bind(
                    &TaskGroup::notifyTaskComplete,
                    this,
                    std::placeholders::_1));
            }
        }
        m_mutex.unlock();

        if (done) {
            runComplete();
            setFlags(TaskFlags::Complete);
        }

        return runLeave(parent, initial);
    }

    virtual void runStart() = 0;

    template <class T> void startTask(T &task) {
        ITask *t = task.run(0, true);

        if (!t) {
            // Task is complete
            taskComplete(t);
        } else {
            // Add to the waiters list
            ITask *root = t->root();
            m_mutex.lock();
            m_subtasks.push_back(root);
            m_mutex.unlock();

            // Then, queue the task
            m_queue->addTask(t, false);
        }
    }

    virtual void taskComplete(ITask *t) { }

    virtual void runComplete() { }

private:
    void notifyTaskComplete(ITask *t) {
        taskComplete(t);
        std::vector<ITask *>::iterator it;

        m_mutex.lock();
        for (it=m_subtasks.begin(); it!=m_subtasks.end(); it++) {
            if ((*it) == t) {
                break;
            }
        }

        if (it == m_subtasks.end()) {
            fprintf(stderr, "Error: failed to find completing task\n");
        } else {
            m_subtasks.erase(it);
        }

        int32_t pending = m_subtasks.size();
        m_mutex.unlock();

        if (!pending) {
            // Re-queue ourselves
            queue();
        }
    }

    using TaskE=std::pair<ITask *, bool>;

private:
    std::vector<ITask *>    m_subtasks;

};

}


