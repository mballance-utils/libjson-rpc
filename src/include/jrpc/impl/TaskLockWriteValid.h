/**
 * TaskLockWriteValid.h
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
#include "jrpc/impl/LockRwValid.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {

class TaskLockWriteValid : public TaskBase {
public:

    TaskLockWriteValid(ITaskQueue *queue, LockRwValid *lock) : 
        TaskBase(queue), m_lock(lock) { }

    TaskLockWriteValid(TaskLockWriteValid *o) : TaskBase(o), m_lock(o->m_lock) { }

    virtual ~TaskLockWriteValid() { }

    virtual ITask *run(ITask *parent, bool initial) override {
        runEnter(parent, initial);
        ITask *ret = this;

        m_lock->lock();
        if (m_lock->m_write_own || m_lock->m_read_own || !m_lock->m_valid) {
            // Someone else owns, or it's invalid
            ret = taskBlock(parent, initial);
            m_lock->m_write_waiters.push_back(ret);
        } else {
            // Got the lock
            setFlags(TaskFlags::Complete);
            m_lock->m_write_own++;
            ret = taskComplete(parent, initial);
        }
        m_lock->unlock();

        return ret;
    }

private:
    LockRwValid         *m_lock;

};

} /* namespace jrpc */


