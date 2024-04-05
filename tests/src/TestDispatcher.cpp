/*
 * TestDispatcher.cpp
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
#include "TestDispatcher.h"
#include "nlohmann/json.hpp"


namespace jrpc {


TestDispatcher::TestDispatcher() {

}

TestDispatcher::~TestDispatcher() {

}

TEST_F(TestDispatcher, valid_req) {
    enableDebug(true);

    TestBase::ReqRspDispatcherLoop h = mkReqDispatcher();

    bool called = false;
    bool idle_called = false;

    h.dispatch->registerMethod("myMethod", [&](IReqMsgUP &m) {
        called = true;
        h.loop->addIdleTask([&]() {
            idle_called = true;
        });
        return IRspMsgUP(m_factory->mkRspMsgSuccess(
            m->getId(),
            {"a", "foo"}
        ));
    });

    nlohmann::json params;

    params["p1"] = 1;

    IRspMsgUP rsp(h.reqrsp->invoke("myMethod", params));
    ASSERT_TRUE(rsp.get());
    ASSERT_EQ(rsp->getId(), "1");
    ASSERT_TRUE(called);

    while (h.loop->process_one_event(1)) {
        ;
    }

    ASSERT_TRUE(idle_called);

    // Need a ReqResp stream
    // Need a Transport+Dispatcher pair
    // 
    // - Must recognize messages that always have an immediate response
    // - Must recognize those that have a deferred response
    // - Can one message be both?
}

}
