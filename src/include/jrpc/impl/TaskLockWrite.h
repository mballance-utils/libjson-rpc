/**
 * TaskLockWrite.h
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
#include "jrpc/impl/LockRw.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {

class TaskLockWrite : public TaskBase {
public:

    TaskLockWrite(ITaskQueue *queue, LockRw *lock) : 
        TaskBase(queue), m_idx(0), m_lock(lock) { }

    TaskLockWrite(TaskLockWrite *o) : 
        TaskBase(o), m_idx(o->m_idx), m_lock(o->m_lock) { }

    virtual ~TaskLockWrite() { }

    virtual ITask *clone() { return new TaskLockWrite(this); }

    virtual ITask *run(ITask *parent, bool initial) override {
        runEnter(parent, initial);
        ITask *ret = this;

        m_lock->lock();
        if (m_lock->m_write_own || m_lock->m_read_own) {
            // Someone else owns 
            ret = taskBlock(parent, initial);
            m_lock->m_write_waiters.push_back(ret);
            m_lock->unlock();
        } else {
            // Got the lock
            setFlags(TaskFlags::Complete);
            m_lock->m_write_own++;

            m_lock->unlock();
            ret = taskComplete(parent, initial);
        }

        return ret;
    }

private:
    int32_t             m_idx;
    LockRw              *m_lock;

};

} /* namespace jrpc */


