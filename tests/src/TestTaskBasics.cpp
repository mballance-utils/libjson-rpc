/*
 * TestTaskBasics.cpp
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
#include "TestTaskBasics.h"


namespace jrpc {


TestTaskBasics::TestTaskBasics() {

}

TestTaskBasics::~TestTaskBasics() {

}

TEST_F(TestTaskBasics, SingleTaskNonBlock) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue) : TaskBase(queue) { }
        virtual ITask *clone() override {
            return new MyTask(m_queue);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            setResult(TaskResult(20));
            setFlags(TaskFlags::Complete);

            return runLeave(parent, initial);
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    MyTask t(infra.queue);
    ITask *next = t.run(0, true);

    ASSERT_FALSE(next);
    ASSERT_EQ(t.getResult().val.si, 20);
}

TEST_F(TestTaskBasics, SingleTaskBlock) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue) : TaskBase(queue) { }
        virtual ITask *clone() override {
            return new MyTask(m_queue);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            if (!initial) {
                setResult(TaskResult(20));
                setFlags(TaskFlags::Complete);
            }

            return runLeave(parent, initial);
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    MyTask t(infra.queue);
    ITask *next = t.run(0, true);
    ASSERT_TRUE(next);

    ITask *next2 = next->run(0, false);
    ASSERT_FALSE(next2);

    ASSERT_EQ(next->getResult().val.si, 20);
}

TEST_F(TestTaskBasics, TwoTaskNonBlock) {
    class MyTask2 : public TaskBase {
    public:
        MyTask2(ITaskQueue *queue) : TaskBase(queue) { }
        virtual ITask *clone() override {
            return new MyTask2(m_queue);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            setResult(TaskResult(20));
            setFlags(TaskFlags::Complete);

            return runLeave(parent, initial);
        }
    };

    class MyTask1 : public TaskBase {
    public:
        MyTask1(ITaskQueue *queue) : TaskBase(queue) { }
        virtual ITask *clone() override {
            return new MyTask1(m_queue);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            ITask *next = MyTask2(m_queue).run(this, initial);

            if (!next) {
                setResult(TaskResult(20));
                setFlags(TaskFlags::Complete);
            }

            return runLeave(parent, initial);
        }
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    MyTask1 t(infra.queue);
    ITask *next = t.run(0, true);

    ASSERT_FALSE(next);
    ASSERT_EQ(t.getResult().val.si, 20);
}

TEST_F(TestTaskBasics, TwoTaskBlockInner) {
    class MyTask2 : public TaskBase {
    public:
        MyTask2(ITaskQueue *queue) : TaskBase(queue), m_step(0) { }
        MyTask2(MyTask2 *o) : TaskBase(o), m_step(o->m_step) { }
        virtual ITask *clone() override {
            return new MyTask2(this);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            switch (m_step) {
                case 0:
                    fprintf(stdout, "MyTask2: step0\n");
                    m_step = 1;
                    break;
                case 1: {
                    fprintf(stdout, "MyTask2: step1\n");
                    fprintf(stdout, "MyTask2 done\n");
                    setResult(TaskResult(20));
                    setFlags(TaskFlags::Complete);
                }
            }

            return runLeave(parent, initial);
        }
        int32_t m_step;
    };

    class MyTask1 : public TaskBase {
    public:
        MyTask1(ITaskQueue *queue) : TaskBase(queue), m_step(0) { }
        MyTask1(MyTask1 *o) : TaskBase(o), m_step(o->m_step) { }
        virtual ITask *clone() override {
            return new MyTask1(this);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            switch (m_step) {
                case 0: {
                    fprintf(stdout, "MyTask1: step0\n");
                    m_step = 1;
                    if (MyTask2(m_queue).run(this, initial)) {
                        break;
                    }
                }
                case 1: {
                    fprintf(stdout, "MyTask1: step1\n");
                    // Only run once MyTask2 is complete
                    m_step = 2;
                    break;
                }
                case 2: {
                    fprintf(stdout, "MyTask1: step2\n");
                    setResult(TaskResult(getResult().val.si+5));
                    setFlags(TaskFlags::Complete);
                }
            }

            return runLeave(parent, initial);
        }
        int32_t m_step;
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    MyTask1 t(infra.queue);
    ITask *next = t.run(0, true);

    ASSERT_TRUE(next);

    ITask *next2 = next->run(0, false);
    ASSERT_TRUE(next2);

    ITask *next3 = next2->run(0, false);
    ASSERT_FALSE(next3);

    ASSERT_EQ(t.getResult().val.si, 25);
}

}
