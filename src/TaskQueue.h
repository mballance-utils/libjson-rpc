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
#include <condition_variable>
#include <mutex>
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "jrpc/ITask.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/ITaskScheduler.h"
#include "jrpc/impl/TaskLambda.h"

namespace jrpc {



class TaskQueue : public virtual ITaskQueue {
public:
    TaskQueue(dmgr::IDebugMgr *dmgr, ITaskScheduler *sched);

    virtual ~TaskQueue();

    virtual void setScheduler(ITaskScheduler *scheduler) override {
        m_scheduler = scheduler;
    }

    virtual bool havePending() override;

    virtual void addTask(ITask *task, bool owned) override;

    virtual void queueTask(ITask *task) override;

    virtual void scheduleTask(ITask *task, uint64_t n_us) override;
    
    virtual void run();

    virtual bool runOneTask() override;

    virtual bool runOneWorkerTask() override;

private:
    using TaskE=std::pair<ITask *, bool>;

private:
    static dmgr::IDebug                 *m_dbg;
    std::mutex                          m_mutex;
    std::condition_variable             m_cond;
    jrpc::ITaskScheduler                *m_scheduler;
    bool                                m_idle_scheduled;
    std::vector<TaskE>                  m_queue;
    int32_t                             m_pending;
    bool                                m_closing;
    std::vector<ITaskQueueWorkerUP>     m_workers;
};

}


