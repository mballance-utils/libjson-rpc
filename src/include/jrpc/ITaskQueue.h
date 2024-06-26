/**
 * ITaskQueue.h
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
#include "jrpc/ITask.h"
#include "jrpc/ITaskQueueWorker.h"
#include "jrpc/ITaskScheduler.h"

namespace jrpc {

class ITaskQueue;
using ITaskQueueUP=std::unique_ptr<ITaskQueue>;
class ITaskQueue {
public:

    virtual ~ITaskQueue() { }

    virtual void setScheduler(ITaskScheduler *scheduler) = 0;

    virtual bool havePending() = 0;

    virtual void addTask(ITask *task, bool owned) = 0;

    // Called by the scheduler to add tasks for evaluation
    virtual void queueTask(ITask *task) = 0;

    virtual void scheduleTask(ITask *task, uint64_t n_us) = 0;

    virtual bool runOneTask() = 0;

    virtual bool runOneWorkerTask() = 0;

};

} /* namespace jrpc */


