/**
 * Semaphore.h
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
#include "jrpc/ISemaphore.h"

namespace jrpc {



class Semaphore : public virtual ISemaphore {
public:
    Semaphore(int32_t ic);

    virtual ~Semaphore();

    virtual void lock() override {
        m_mutex.lock();
    }

    virtual void unlock() override {
        m_mutex.unlock();
    }

    virtual void put(int32_t n=1) override;

    virtual bool try_get(int32_t n=1) override;

    virtual bool has(int32_t n=1) override;

    virtual void addWaiter(ITask *task) override;

private:
    std::mutex                      m_mutex;
    int32_t                         m_count;
    std::vector<ITask *>            m_waiters;
};

}


