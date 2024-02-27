/**
 * TaskBase.h
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

namespace jrpc {

class TaskBase : public virtual ITask {
public:

    TaskBase(ITaskGroup *group) : 
        m_group(group), m_flags(TaskFlags::NoFlags) { }

    TaskBase(TaskBase *o) :
        m_group(o->m_group), m_result(o->moveResult()), 
        m_flags(o->m_flags) { }

    virtual ~TaskBase() { }

    virtual ITaskGroup *group() override {
        return m_group;
    }

    virtual bool hasFlags(TaskFlags flags) override {
        return (m_flags & flags) == flags;
    }

    virtual void setFlags(TaskFlags flags) override {
        m_flags = (m_flags | flags);
    }

    virtual void clrFlags(TaskFlags flags) override {
        m_flags = (m_flags & ~flags);
    }

    virtual void setResult(const TaskResult &r) override {
        m_result = r;
    }

    virtual void setResult(TaskResult &r) override {
        m_result = r;
    }

    virtual const TaskResult &getResult() const {
        return m_result;
    }

    virtual TaskResult &moveResult() override {
        return m_result;
    }

protected:
    ITaskGroup              *m_group;
    TaskResult              m_result;
    TaskFlags               m_flags;
};

} /* namespace jrpc */


