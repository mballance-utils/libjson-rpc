/**
 * LockRwData.h
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
#include <memory>
#include "jrpc/ITask.h"
#include "jrpc/impl/LockRw.h"

namespace jrpc {


class LockRwData : public LockRw {
public:
    friend class TaskLockReadData;
    friend class TaskLockWriteData;

    LockRwData(void *init=0) : m_data(init), m_update_task(0) { }

    virtual ~LockRwData() { }

    /**
     * Set data and unlock the mutex
     */
    virtual void unlock_write_data(void *data, const std::function<ITask *()> &mk_update_task) {
        m_mutex.lock();
        if (m_write_own) {
            m_write_own--;
        }
        
        m_data = data;

        if (!m_data && !m_update_task) {
            m_update_task = mk_update_task();
            m_update_task->addCompletionMon([this](ITask *t) {
                m_mutex.lock();
                m_update_task = 0;
                m_mutex.unlock();
            });
        }

        // Notify any waiting tasks, prioritizing tasks that want write access
        if (m_write_waiters.size() && !m_write_own && !m_read_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_write_waiters.begin();
                it!=m_write_waiters.end(); it++) {
                (*it)->queue();
            }
            m_write_waiters.clear();
        } else if (m_read_waiters.size() && !m_write_own && m_data) {
            // Only notify read waiters if data is present
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_read_waiters.begin();
                it!=m_read_waiters.end(); it++) {
                (*it)->queue();
            }
            m_read_waiters.clear();
        }
        m_mutex.unlock();        
    }

    /**
     * Set data and unlock the mutex
     */
    virtual void unlock_write_data(void *data) {
        m_mutex.lock();
        if (m_write_own) {
            m_write_own--;
        }
        
        m_data = data;

        // Notify any waiting tasks, prioritizing tasks that want write access
        if (m_write_waiters.size() && !m_write_own && !m_read_own) {
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_write_waiters.begin();
                it!=m_write_waiters.end(); it++) {
                (*it)->queue();
            }
            m_write_waiters.clear();
        } else if (m_read_waiters.size() && !m_write_own && m_data) {
            // Only notify read waiters if data is present
            for (std::vector<jrpc::ITask *>::const_iterator
                it=m_read_waiters.begin();
                it!=m_read_waiters.end(); it++) {
                (*it)->queue();
            }
            m_read_waiters.clear();
        }
        m_mutex.unlock();        
    }

    void *getData() {
        return m_data;
    }

    template <class T> T *getDataT() {
        return reinterpret_cast<T *>(m_data);
    }

private:
    void                *m_data;
    ITask               *m_update_task;

};

} /* namespace jrpc */


