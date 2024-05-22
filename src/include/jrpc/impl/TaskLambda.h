/**
 * TaskLambda.h
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
#include <functional>
#include "jrpc/ITaskQueue.h"
#include "jrpc/ITaskStack.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {


class TaskLambda : public virtual TaskBase {
public:
    TaskLambda(
        ITaskQueue                                  *queue,
        const std::function<ITask *(ITask *, bool)>  &f) : 
        TaskBase(queue), m_func(f), m_initial(true) { }

    TaskLambda(TaskLambda *o) : TaskBase(o), 
        m_func(o->m_func), m_initial(false) { }

    virtual ~TaskLambda() { }

    virtual ITask *clone() override {
        return new TaskLambda(this);
    }

    virtual ITask *run(ITask *parent, bool initial) override {
        initial = m_initial;
        runEnter(parent, initial);

        if (initial) {
            ITask *ret = m_func(this, initial);
            if (!ret || ret->done()) {
                setFlags(jrpc::TaskFlags::Complete);
            }
        } else {
            setFlags(jrpc::TaskFlags::Complete);
        }

        return runLeave(parent, initial);
    }

protected:
    bool                                                m_initial;
    std::function<ITask *(ITask *, bool)>              m_func;

};

}


