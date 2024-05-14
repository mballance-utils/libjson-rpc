/**
 * LockRw.h
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
#include "jrpc/ITask.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {



class LockRw {
public:
    friend class TaskLockRead;
    friend class TaskLockWrite;

    LockRw() : m_write_own(0), m_read_own(0) { }

    virtual ~LockRw() { }

    virtual void unlock_write() {
        m_mutex.lock();
        if (m_write_own) {
            m_write_own--;
        }
        if (m_write_waiters.size() && !m_write_own && !m_read_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_write_waiters.begin();
                it!=m_write_waiters.end(); it++) {
                (*it)->queue();
            }
            m_write_waiters.clear();
        } else if (m_read_waiters.size() && !m_write_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_read_waiters.begin();
                it!=m_read_waiters.end(); it++) {
                (*it)->queue();
            }
            m_read_waiters.clear();
        }
        m_mutex.unlock();
    }

    virtual void unlock_read() {
        m_mutex.lock();
        if (m_read_own) {
            m_read_own--;
        }
        if (m_write_waiters.size() && !m_write_own && !m_read_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_write_waiters.begin();
                it!=m_write_waiters.end(); it++) {
                (*it)->queue();
            }
            m_write_waiters.clear();
        } else if (m_read_waiters.size() && !m_write_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_read_waiters.begin();
                it!=m_read_waiters.end(); it++) {
                (*it)->queue();
            }
            m_read_waiters.clear();
        }
        m_mutex.unlock();
    }

    virtual void lock() {
        m_mutex.lock();
    }

    virtual void unlock() {
        m_mutex.unlock();
    }

protected:
    std::mutex                      m_mutex;
    std::vector<ITask *>            m_write_waiters;
    std::vector<ITask *>            m_read_waiters;
    int32_t                         m_write_own;
    int32_t                         m_read_own;

};

} /* namespace jrpc */


