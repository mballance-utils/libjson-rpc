/**
 * TaskSemWait.h
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
#include "jrpc/ISemaphore.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {



class TaskSemWait : public TaskBase {
public:
    TaskSemWait(ITaskQueue *queue, ISemaphore *sem, int32_t n=1) :
        TaskBase(queue), m_sem(sem), m_n(n) { }

    TaskSemWait(TaskSemWait *o) : TaskBase(o), m_sem(o->m_sem), m_n(o->m_n) { }

    virtual ~TaskSemWait() { }

    virtual ITask *clone() override {
        return new TaskSemWait(this);
    }

    virtual ITask *run(ITask *parent, bool initial) {
        ITask *ret = 0;
        runEnter(parent, initial);

        m_sem->lock();
        if (m_sem->has(m_n)) {
            setFlags(TaskFlags::Complete);

            if (!initial) {
                ITask *next = m_parent;

                delete this;

                if (next) {
                    ret = next->run(0, false);
                }
            }
        } else {
            ITask *waiter;
            if (initial) {
                ret = clone();
                waiter = ret;

                // Update links such that we point at the chain
                // that is being built
                if (parent) {
                    // Non-root task
                    dynamic_cast<TaskBase *>(parent)->m_child = ret;
                } else {
                    // This is the root task
                    m_parent = 0;

                    // Go find the actual tail, since we're now at root
                    ret = ret->tail();
                }
            } else {
                waiter = this;
            }
            m_sem->addWaiter(waiter);
        }
        m_sem->unlock();

        return ret;
    }

protected:
    ISemaphore              *m_sem;
    int32_t                 m_n;
};

} /* namespace jrpc */


