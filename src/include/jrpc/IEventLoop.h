/**
 * IEventLoop.h
 *
 * Copyright 2022 Matthew Ballance and Contributors
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
#include <functional>
#include <memory>
#include <stdint.h>
#include "jrpc/ITaskScheduler.h"

namespace jrpc {

class ITaskQueue;


class IEventLoop;
using IEventLoopUP=std::unique_ptr<IEventLoop>;
class IEventLoop : public virtual ITaskScheduler {
public:

    virtual ~IEventLoop() { }

    virtual int32_t process_one_event(int32_t timeout_ms) = 0;

    virtual void setTaskQueue(ITaskQueue *q) = 0;

    virtual void addAfterTask(
        std::function<void ()>  task,
        int32_t                 ms) = 0;

    virtual void addFdReadTask(
        std::function<void ()>  task,
        int32_t                 fd) = 0;

    virtual void addFdWriteTask(
        std::function<void ()>  task,
        int32_t                 fd) = 0;

};

} /* namespace jrpc */


