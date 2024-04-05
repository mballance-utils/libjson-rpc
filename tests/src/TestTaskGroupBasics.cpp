/*
 * TestTaskGroupBasics.cpp
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
#include <unistd.h>
#include <thread>
#include "TestTaskGroupBasics.h"


namespace jrpc {


TestTaskGroupBasics::TestTaskGroupBasics() {

}

TestTaskGroupBasics::~TestTaskGroupBasics() {

}

TEST_F(TestTaskGroupBasics, ImmComplete) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue) : TaskBase(queue) { }
        MyTask(MyTask *o) : TaskBase(o) { }

        virtual MyTask *clone() override {
            return new MyTask(this);
        }

        virtual ITask *run(ITask *parent, bool initial) {
            setResult(TaskResult(20));
            setFlags(TaskFlags::Complete);
        }
    };

    class MyTaskGroup : public TaskGroup {
    public:
        MyTaskGroup(ITaskQueue *queue) : TaskGroup(queue) { }
        MyTaskGroup(MyTaskGroup *o) : TaskGroup(o) { }

        virtual MyTaskGroup *clone() override {
            return new MyTaskGroup(this);
        }

        virtual void runStart() {
            for (uint32_t i=0; i<20; i++) {
                MyTask t(m_queue);
                startTask(t);
            }
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    MyTaskGroup t(infra.queue);

    ITask *next = t.run(0, true);
    ASSERT_FALSE(next);
}

TEST_F(TestTaskGroupBasics, IncrComplete) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue) : TaskBase(queue), m_step(0) { }
        MyTask(MyTask *o) : TaskBase(o), m_step(o->m_step) { }

        virtual MyTask *clone() override {
            return new MyTask(this);
        }

        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);
            switch (m_step) {
                case 0: {
                    m_step = 1;
                    break;
                }
                case 1: {
                    m_step = 2;
                    setResult(TaskResult(20));
                    setFlags(TaskFlags::Complete);
                }
            }
            return runLeave(parent, initial);
        }

        int32_t             m_step;
    };

    class MyTaskGroup : public TaskGroup {
    public:
        MyTaskGroup(ITaskQueue *queue) : TaskGroup(queue) { }
        MyTaskGroup(MyTaskGroup *o) : TaskGroup(o) { }

        virtual MyTaskGroup *clone() override {
            return new MyTaskGroup(this);
        }

        virtual void runStart() {
            for (uint32_t i=0; i<20; i++) {
                MyTask t(m_queue);
                startTask(t);
            }
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    ASSERT_TRUE(infra.queue);
    MyTaskGroup t(infra.queue);

    ITask *next = t.run(0, true);
    ASSERT_TRUE(next);

    while (infra.queue->runOneTask()) {
        ;
    }
}

void worker(ITaskQueue *queue, int32_t id) {
    while (queue->runOneTask()) {
        fprintf(stdout, "Thread %d\n", id);
    }
}

TEST_F(TestTaskGroupBasics, IncrCompleteParDispatch) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue) : TaskBase(queue), m_step(0) { }
        MyTask(MyTask *o) : TaskBase(o), m_step(o->m_step) { }

        virtual MyTask *clone() override {
            return new MyTask(this);
        }

        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);
            switch (m_step) {
                case 0: {
                    m_step = 1;
                    break;
                }
                case 1: {
                    m_step = 2;
                    usleep(1000);
                    setResult(TaskResult(20));
                    setFlags(TaskFlags::Complete);
                }
            }
            return runLeave(parent, initial);
        }

        int32_t             m_step;
    };

    class MyTaskGroup : public TaskGroup {
    public:
        MyTaskGroup(ITaskQueue *queue) : TaskGroup(queue) { }
        MyTaskGroup(MyTaskGroup *o) : TaskGroup(o) { }

        virtual MyTaskGroup *clone() override {
            return new MyTaskGroup(this);
        }

        virtual void runStart() {
            for (uint32_t i=0; i<20; i++) {
                MyTask t(m_queue);
                startTask(t);
            }
        }

        virtual void runComplete() {
            fprintf(stdout, "Run complete\n");
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    ASSERT_TRUE(infra.queue);
    MyTaskGroup t(infra.queue);

    ITask *next = t.run(0, true);
    ASSERT_TRUE(next);


    std::thread *workers[4];
    for (uint32_t i=0; i<sizeof(workers)/sizeof(std::thread *); i++) {
        workers[i] = new std::thread(worker, infra.queue, i);
    }

    // Join on threads
    for (uint32_t i=0; i<sizeof(workers)/sizeof(std::thread *); i++) {
        workers[i]->join();
    }

}

}
