/*
 * Semaphore.cpp
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
#include "Semaphore.h"


namespace jrpc {


Semaphore::Semaphore(int32_t ic) : m_count(ic) {

}

Semaphore::~Semaphore() {

}

void Semaphore::put(int32_t n) {
    m_mutex.lock();
    m_count += n;

    // Unblock all waiters so they can retry
    for (std::vector<ITask *>::const_iterator
        it=m_waiters.begin();
        it!=m_waiters.end(); it++) {
        (*it)->queue();
    }
    m_waiters.clear();

    m_mutex.unlock();
}

bool Semaphore::try_get(int32_t n) {
    bool ret = (m_count >= n);

    if (ret) {
        m_count -= n;
    }

    return ret;
}

bool Semaphore::has(int32_t n) {
    return (n <= m_count);
}

void Semaphore::addWaiter(ITask *task) {
    m_waiters.push_back(task);
}

}
