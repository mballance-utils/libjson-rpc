/**
 * TaskGroup.h
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
#include "jrpc/ITaskGroup.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {


class TaskGroup : public virtual ITaskGroup {
public:
    TaskGroup(ITaskQueue *queue);

    virtual ~TaskGroup();

    virtual void pushTask(ITask *t) override;

    virtual void suspTask() override;

    virtual void wakeTask() override;

    virtual void popTask() override;

    virtual TaskStatus run() override;

    virtual ITask *clone() override { return 0; }

    virtual ITaskGroup *group() override {
        return this;
    }

    virtual bool hasFlags(TaskFlags flags) override;

    virtual void setFlags(TaskFlags flags) override;

    virtual void clrFlags(TaskFlags flags) override;

    virtual void setResult(const TaskResult &r) override;

    virtual void setResult(TaskResult &r) override;

    virtual const TaskResult &getResult() const override;

    virtual TaskResult &moveResult() override;

private:
    using TaskE=std::pair<ITask *, bool>;

private:
    ITaskQueue              *m_queue;
    std::vector<TaskE>      m_task_s;
    TaskFlags               m_flags;
    TaskResult              m_result;

};

}


