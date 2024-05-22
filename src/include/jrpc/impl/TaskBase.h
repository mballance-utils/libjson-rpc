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
#include <mutex>
#include <vector>
#include "jrpc/ITask.h"
#include "jrpc/ITaskDone.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/ITaskStack.h"

namespace jrpc {

class TaskBase : public virtual ITask {
public:

    TaskBase(ITaskQueue *queue) : 
        m_queue(queue), m_parent(0), m_child(0), m_flags(TaskFlags::NoFlags) { }

    TaskBase(TaskBase *o) :
        m_queue(o->m_queue), m_result(o->moveResult()), 
        m_child(o->m_child), m_parent(o->m_parent), 
        m_done(o->m_done.begin(), o->m_done.end()),
        m_flags(o->m_flags) { }

    virtual ~TaskBase() { }

    virtual void runEnter(ITask *parent, bool initial) {
        if (initial) {
            // Link our (initial) selves into the chain
            m_parent = parent;
            if (parent) {
                dynamic_cast<TaskBase *>(parent)->m_child = this;
            }
        }
    }

    ITask *taskComplete(ITask *parent, bool initial) {
        ITask *ret = 0;

        // First things first, notify any watchers
        for (std::vector<std::function<void (ITask *)>>::const_iterator
            it=m_done.begin();
            it!=m_done.end(); it++) {
            (*it)(this);
        }

        if (m_parent) {
            // Always propagate the result up
            m_parent->setResult(moveResult());
            dynamic_cast<TaskBase *>(m_parent)->m_child = 0;
            if (!initial) {
                // If this isn't the first call, then
                // we need to call back up the stack
                // to see where we block next
                ITask *next = m_parent;

                // Since this task is complete, delete it
                delete this;

                ret = next->run(0, false);
            }
        }

        return ret;
    }

    ITask *taskBlock(ITask *parent, bool initial) {
        ITask *ret = this;

        // We need to suspend. 
        if (initial) {
            // Initial, so clone ourselves
            ret = clone();

            if (m_child) {
                dynamic_cast<TaskBase *>(m_child)->m_parent = ret;
            }

            // Update links such that we point at the chain
            // that is being built
            if (parent) {
                // Non-root task
                dynamic_cast<TaskBase *>(parent)->m_child = ret;
            } else {
                // This is the root task
                m_parent = 0;

                // Go find the actual tail, since we're now at root
                ret = ret->tail();
            }
        } else {
            // On a non-initial blocking call, just
            // return ourselves (we've already been cloned)
            ret = this;
        }

        // If the leaf task yielded, then re-queue 
        // immediately
        if (ret && ret->hasFlags(TaskFlags::Yield)
            && ret->hasFlags(TaskFlags::Complete)) {
            ret->queue();
        }

        return ret;
    }

    virtual ITask *runLeave(ITask *parent, bool initial) {
        ITask *ret = 0;
        if (hasFlags(TaskFlags::Complete)) {
            // We're done.
            ret = taskComplete(parent, initial);
        } else {
            ret = taskBlock(parent, initial);
        }
        return ret;
    }

    virtual ITask *parent() const override {
        return m_parent;
    }

    virtual ITask *root() override {
        ITask *ret = this;
        while (ret->parent()) {
            ret = ret->parent();
        }
        return ret;
    }

    virtual ITask *tail() override {
        ITask *ret = this;
        while (dynamic_cast<TaskBase *>(ret)->m_child) {
            ret = dynamic_cast<TaskBase *>(ret)->m_child;
        }
        return ret;
    }

    virtual void addCompletionMon(const std::function<void (ITask *)> &mon) override {
        m_mutex.lock();
        m_done.push_back(mon);
        m_mutex.unlock();
    }

    virtual bool hasFlags(TaskFlags flags) override {
        bool ret;
        m_mutex.lock();
        ret = (m_flags & flags) == flags;
        m_mutex.unlock();
        return ret;
    }

    virtual void setFlags(TaskFlags flags) override {
        m_mutex.lock();
        m_flags = (m_flags | flags);
        m_mutex.unlock();
    }

    virtual void clrFlags(TaskFlags flags) override {
        m_mutex.lock();
        m_flags = (m_flags & ~flags);
        m_mutex.unlock();
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

    virtual void queue() override {
        clrFlags(TaskFlags::Yield|TaskFlags::Blocked);
        m_queue->addTask(this, false);
    }

public:
    // Tracks tasks above and below this one
    ITask                       *m_child;
    ITask                       *m_parent;

protected:
    std::mutex                                    m_mutex;
    ITaskQueue                                    *m_queue;
    std::vector<std::function<void (ITask *)>>    m_done;
    TaskResult                                    m_result;
    TaskFlags                                     m_flags;

};

} /* namespace jrpc */


