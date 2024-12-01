/**
 * ITask.h
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
#include <memory>
#include <string>
#include <stdint.h>

namespace jrpc {

class ITaskParent;

enum class TaskFlags {
    NoFlags  = 0,
    Complete = (1 << 0),
    Error    = (1 << 1),
    Queued   = (1 << 2),
    Yield    = (1 << 3),
    Blocked  = (1 << 4)
};

static inline TaskFlags operator & (const TaskFlags lhs, const TaskFlags rhs) {
    return static_cast<TaskFlags>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

static inline TaskFlags operator | (const TaskFlags lhs, const TaskFlags rhs) {
    return static_cast<TaskFlags>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

static inline TaskFlags operator ~ (const TaskFlags lhs) {
    return static_cast<TaskFlags>(~static_cast<uint32_t>(lhs));
}

class TaskResultP {
public:
    void            *p;
    TaskResultP(void *p) {
        this->p = p;
    }
    virtual ~TaskResultP() { }
};

class TaskResultStrP : public virtual TaskResultP {
public:
    TaskResultStrP(const std::string &v) :
        TaskResultP(new std::string(v)) {
    }
    virtual ~TaskResultStrP() {
        delete reinterpret_cast<std::string *>(p);
    }
};

enum class TaskResultFlags {
    NoFlags = 0,
    Owned   = (1 << 0),
    Pointer = (1 << 1)
};

static inline TaskResultFlags operator |(const TaskResultFlags &l, const TaskResultFlags &r) {
    return static_cast<TaskResultFlags>(
        static_cast<uint32_t>(l) | static_cast<uint32_t>(r));
}

static inline TaskResultFlags operator &(const TaskResultFlags &l, const TaskResultFlags &r) {
    return static_cast<TaskResultFlags>(
        static_cast<uint32_t>(l) & static_cast<uint32_t>(r));
}


class TaskResult {
public:
    union {
        int64_t         si;
        bool            b;
        void            *p;
    } val;
    TaskResultFlags     flags;

    TaskResult() {
        val.si = 0;
        flags = TaskResultFlags::NoFlags;
    }

    TaskResult(int64_t si) {
        val.si = si;
        flags = TaskResultFlags::NoFlags;
    }

    TaskResult(void *p, bool owned) {
        val.p = p;
        flags = TaskResultFlags::Pointer | (owned ? TaskResultFlags::Owned : TaskResultFlags::NoFlags);
    }

/*
    TaskResult(bool b) {
        val.b = b;
        p = 0;
        owned = false;
    }
 */

    ~TaskResult() {
    }

};

enum class TaskStatus {
    Done,
    Yield,
    Blocked
};

class ITaskDone;

class ITask;
using ITaskUP=std::unique_ptr<ITask>;
class ITask {
public:

    virtual ~ITask() { }

    /**
     * @brief Runs task behavior. 
     * 
     * @return true -- more work to be done
     * @return false -- work complete
     */
    virtual ITask *run(ITask *parent, bool initial) = 0;

    virtual ITask *clone() = 0;

    virtual ITask *parent() const = 0;

    virtual ITask *root() = 0;

    virtual ITask *tail() = 0;

    virtual void addCompletionMon(const std::function<void (ITask *)> &mon) = 0;

    bool done() { return hasFlags(TaskFlags::Complete); }

    virtual bool hasFlags(TaskFlags flags) = 0;

    virtual void setFlags(TaskFlags flags) = 0;

    virtual void clrFlags(TaskFlags flags) = 0;

    virtual void setResult(const TaskResult &r) = 0;

    virtual void setResult(TaskResult &r) = 0;

    virtual const TaskResult &getResult() const = 0;

    virtual TaskResult &moveResult() = 0;

    virtual void queue() = 0;

};

} /* namespace jrpc */


