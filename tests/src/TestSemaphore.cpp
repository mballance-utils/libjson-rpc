/*
 * TestSemaphore.cpp
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
#include "TestSemaphore.h"


namespace jrpc {


TestSemaphore::TestSemaphore() {

}

TestSemaphore::~TestSemaphore() {

}

TEST_F(TestSemaphore, WaitNop) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue, ISemaphore *sem) : 
            TaskBase(queue), m_sem(sem), m_step(0) { }
        MyTask(MyTask *o) : TaskBase(o), m_sem(o->m_sem), m_step(o->m_step) { }
        virtual ITask *clone() override {
            return new MyTask(this);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            switch (m_step) {
            case 0: {
                m_step = 1;
                if (TaskSemWait(m_queue, m_sem).run(this, true)) {
                    break;
                }
            }
            case 1: {
                setResult(TaskResult(20));
                setFlags(TaskFlags::Complete);
            }
            }

            return runLeave(parent, initial);
        }

        ISemaphore      *m_sem;
        int32_t         m_step;
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    ISemaphore *sem = m_factory->mkSemaphore(1);

    ITask *next = MyTask(infra.queue, sem).run(0, true);
    ASSERT_FALSE(next);

}

TEST_F(TestSemaphore, WaitActive) {
    class MyTask : public TaskBase {
    public:
        MyTask(ITaskQueue *queue, ISemaphore *sem) : 
            TaskBase(queue), m_sem(sem), m_step(0) { }
        MyTask(MyTask *o) : TaskBase(o), m_sem(o->m_sem), m_step(o->m_step) { }
        virtual ITask *clone() override {
            return new MyTask(this);
        }
        virtual ITask *run(ITask *parent, bool initial) {
            runEnter(parent, initial);

            switch (m_step) {
            case 0: {
                m_step = 1;
                if (TaskSemWait(m_queue, m_sem).run(this, true)) {
                    break;
                }
            }
            case 1: {
                setResult(TaskResult(20));
                setFlags(TaskFlags::Complete);
            }
            }

            return runLeave(parent, initial);
        }

        ISemaphore      *m_sem;
        int32_t         m_step;
    };

    ReqRspDispatcherLoop infra = mkReqDispatcher();
    ISemaphore *sem = m_factory->mkSemaphore(0);

    ITask *next = MyTask(infra.queue, sem).run(0, true);
    ASSERT_TRUE(next);

    sem->put(1);

    while (infra.queue->runOneTask()) {
        ;
    }
    
//    ITask *next = MyTask(infra.queue, sem).run(0, true);
//    ASSERT_TRUE(next);

}

}
