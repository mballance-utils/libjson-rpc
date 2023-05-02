/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * MessageDispatcher.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */
#include "dmgr/impl/DebugMacros.h"
#include "MessageDispatcher.h"
#include "ReqMsg.h"
#include "nlohmann/json.hpp"

namespace jrpc {

MessageDispatcher::MessageDispatcher(IFactory *factory) 
    : m_factory(factory) {
	// TODO Auto-generated constructor stub
    DEBUG_INIT("MessageDispatcher", factory->getDebugMgr());
}

MessageDispatcher::~MessageDispatcher() {
	// TODO Auto-generated destructor stub
}

void MessageDispatcher::init(IMessageTransport *peer) {
    m_peer = peer;
}

void MessageDispatcher::registerMethod(
		const std::string							&method,
		std::function<IRspMsgUP(IReqMsgUP &)>	impl) {
	m_method_m.insert({method, impl});
}

void MessageDispatcher::send(const nlohmann::json &msg) {
	DEBUG_ENTER("send");
	std::map<std::string,std::function<IRspMsgUP(IReqMsgUP &)>>::iterator it;
    int32_t id = -1;
    id = msg["id"];

    if (msg.contains("method")) {
        const std::string &method = msg["method"];
    	if ((it=m_method_m.find(method)) != m_method_m.end()) {
		    DEBUG("==> calling method impl");
            IReqMsgUP req(new ReqMsg(id, method, msg["params"]));
    		IRspMsgUP rsp(it->second(req));
            if (rsp) {
                nlohmann::json rsp_m;

                rsp_m["jsonrpc"] = "2.0";
                rsp_m["id"] = id;

                if (rsp->getErrorCode() != -1) {
                    // Sending back an error response
                    nlohmann::json &error = rsp_m["error"];
                    error["code"] = rsp->getErrorCode();
                    error["message"] = rsp->getErrorMsg();
                    error["data"] = rsp->getResult();
                } else {
                    // Sending back a success
                    rsp_m["result"] = rsp->getResult();
                 }

                DEBUG_ENTER("Send response");
                m_peer->send(rsp_m);
                DEBUG_LEAVE("Send response");
            } else {
                DEBUG("No response");
            }

    		DEBUG("<== calling method impl");
	    } else {
    		// Send back an error response with code -32601 (no method)
            DEBUG("Error: no method \"%s\" registered", method.c_str());
    	}
    } else {
        // Response (no method)
        if (m_handler) {
            IRspMsgUP rsp(m_factory->mkRspMsg(msg));

            m_handler(id, rsp);
        }
    }
	DEBUG_LEAVE("send");
}

dmgr::IDebug *MessageDispatcher::m_dbg = 0;

} /* namespace lls */
