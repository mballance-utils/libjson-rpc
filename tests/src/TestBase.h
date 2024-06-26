/**
 * TestBase.h
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
#include "jrpc/IFactory.h"
#include "jrpc/IMessageTransport.h"
#include "jrpc/impl/TaskBase.h"
#include "jrpc/impl/TaskGroup.h"
#include "jrpc/impl/TaskSemWait.h"
#include "gtest/gtest.h"

namespace jrpc {



class TestBase : public ::testing::Test {
public:
    struct ReqRspDispatcherLoop {
        IMessageRequestResponseStream       *reqrsp;
        IMessageDispatcher                  *dispatch;
        IEventLoop                          *loop;
        ITaskQueue                          *queue;
    };
public:
    TestBase();

    virtual ~TestBase();

    virtual void SetUp() override;

    std::pair<int32_t, int32_t> mkClientServerPair();

    ReqRspDispatcherLoop mkReqDispatcher();

    void enableDebug(bool en);

protected:
    std::pair<jrpc::IMessageTransportUP, jrpc::IMessageTransportUP> mkTransportPair(
        jrpc::IEventLoop *loop);

protected:
    IFactory                *m_factory;

};

}


