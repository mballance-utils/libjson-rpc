/**
 * TaskRunTasklist.h
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
#include <mutex>
#include <vector>
#include "jrpc/impl/TaskBase.h"

namespace jrpc {



class TaskRunTasklist : public TaskBase {
public:

    TaskRunTasklist(
        jrpc::ITaskQueue            *queue, 
        const std::vector<ITask *>  &tasks) : TaskBase(queue),
        m_tasks(tasks.begin(), tasks.end()) { }

    TaskRunTasklist(TaskRunTasklist *o) : TaskBase(o),
        m_tasks(o->m_tasks.begin(), o->m_tasks.end()) { }

    virtual ~TaskRunTasklist() { }

    virtual TaskRunTasklist *clone() override {
        return new TaskRunTasklist(this);
    }

    virtual ITask *run(ITask *parent, bool initial) {
        fprintf(stdout, "TaskRunTasklist::run\n");
        fflush(stdout);
        runEnter(parent, initial);

        ITask *ret;
        if (initial) {
            TaskRunTasklist *rtl = dynamic_cast<TaskRunTasklist *>(runLeave(parent, initial));

            for (std::vector<ITask *>::const_iterator
                it=m_tasks.begin();
                it!=m_tasks.end(); it++) {
                
                (*it)->addCompletionMon([rtl](ITask *t) {
                    rtl->taskDone(t);
                });
                m_queue->addTask(*it, false);
            }
            ret = rtl;
        } else {
            setFlags(TaskFlags::Complete);
            ret = runLeave(parent, initial);
        }

        return ret;
    }

private:
    void taskDone(ITask *t) {
        bool done = false;
        m_mutex.lock();
        for (std::vector<ITask *>::const_iterator
            it=m_tasks.begin();
            it!=m_tasks.end(); it++) {
            if (t == *it) {
                m_tasks.erase(it);
                break;
            }
        }

        fprintf(stdout, "taskDone: m_tasks=%d\n", m_tasks.size());
        fflush(stdout);

        done = (m_tasks.size() == 0);
        m_mutex.unlock();

        if (done) {
            // Time to reschedule the parent task
            m_queue->addTask(this, false);
        }
    }

private:
    std::mutex                      m_lock;
    std::vector<ITask *>            m_tasks;

};

} /* namespace jrpc */


