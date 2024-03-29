/**
 * TaskQueue.h
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
#include "jrpc/IEventLoop.h"
#include "jrpc/ITask.h"
#include "jrpc/ITaskQueue.h"

namespace jrpc {



class TaskQueue : public virtual ITaskQueue {
public:
    TaskQueue(jrpc::IEventLoop *loop);

    virtual ~TaskQueue();

    virtual void addTask(ITask *task, bool owned) override;
    
    virtual void addTaskPreempt(ITask *task, bool owned) override;

    virtual ITaskGroup *mkTaskGroup() override;

    virtual void run();

private:
    using TaskE=std::pair<ITask *, bool>;

private:
    bool                        m_idle_scheduled;
    jrpc::IEventLoop            *m_loop;
    std::vector<TaskE>          m_queue;
};

}


