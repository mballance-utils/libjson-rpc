/**
 * EventLoop.h
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
#include <mutex>
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IEventLoop.h"

namespace jrpc {

class ITaskQueue;


class EventLoop : public virtual IEventLoop {
public:
    EventLoop(dmgr::IDebugMgr *dmgr);

    virtual ~EventLoop();

    virtual int32_t process_one_event(int32_t timeout_ms) override;

    virtual void setTaskQueue(ITaskQueue *q) override {
        m_task_q = q;
    }

    virtual void addAfterTask(
        std::function<void ()>  task,
        int32_t                 ms) override;

    virtual void addFdReadTask(
        std::function<void ()>  task,
        int32_t                 fd) override;

    virtual void addFdWriteTask(
        std::function<void ()>  task,
        int32_t                 fd) override;

    virtual void scheduleTask(ITask *task, uint64_t n_us) override;

    virtual void cancelSchedule(ITask *task) override;
    

private:
    using FdTask=std::pair<int32_t,std::function<void()>>;
    using TimedTask=std::pair<int32_t,std::function<void()>>;
    using IdleTask=std::function<void()>;
    using TimeEntry=std::pair<uint64_t, ITask *>;
private:
    static dmgr::IDebug                 *m_dbg;
    std::mutex                          m_mutex;
    std::vector<FdTask>                 m_read_tasks;
    std::vector<FdTask>                 m_write_tasks;
    std::vector<TimedTask>              m_timed_tasks;
    std::vector<IdleTask>               m_idle_tasks;
    ITaskQueue                          *m_task_q;
    std::vector<TimeEntry>              m_time_q;

};

}


