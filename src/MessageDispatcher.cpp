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

MessageDispatcher::MessageDispatcher(
    IFactory        *factory,
    ITaskQueue      *queue) : m_factory(factory), m_queue(queue) {
	// TODO Auto-generated constructor stub
    DEBUG_INIT("MessageDispatcher", factory->getDebugMgr());
    DispatchTask::m_dbg = m_dbg;
}

MessageDispatcher::~MessageDispatcher() {
	// TODO Auto-generated destructor stub
}

void MessageDispatcher::init(IMessageTransport *peer) {
    m_peer = peer;
}

void MessageDispatcher::registerMethod(
		const std::string				&method,
        IMessageDispatcher::MethodF     method_f) {
	m_method_m.insert({method, method_f});
}

void MessageDispatcher::send(const nlohmann::json &msg) {
	DEBUG_ENTER("send");
    std::string id = "-1";

    if (msg.contains("id")) {
        if (msg["id"].is_string()) {
            id = msg["id"].get<std::string>();
        } else if (msg["id"].is_number()) {
            int32_t id_i = msg["id"].get<int32_t>();
            char tmp[16];
            sprintf(tmp, "%d", id_i);
            id = tmp;
        }
    }

    if (msg.contains("method")) {
        const std::string &method = msg["method"];
        IReqMsgUP req(new ReqMsg(id, method, msg["params"]));

        if (m_queue) {
            DEBUG("Queue message-dispatch task");
            m_queue->addTask(new DispatchTask(0, this, req), true);
        } else {
            dispatch(req);
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

void MessageDispatcher::dispatch(IReqMsgUP &req) {
    DEBUG_ENTER("dispatch");
	std::map<std::string,IMessageDispatcher::MethodF>::iterator it;
 	if ((it=m_method_m.find(req->getMethod())) != m_method_m.end()) {
//        DEBUG("==> calling method impl");
        const std::string &id = req->getId();
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
        DEBUG("Error: no method \"%s\" registered", req->getMethod().c_str());

        nlohmann::json rsp_m;
        rsp_m["jsonrpc"] = "2.0";
        rsp_m["id"] = req->getId();

        nlohmann::json &error = rsp_m["error"];
        error["code"] = -32601; // no method
        error["message"] = "No such method";

        DEBUG_ENTER("Send error response");
        m_peer->send(rsp_m);
        DEBUG_LEAVE("Send error response");
  	}
    DEBUG_LEAVE("dispatch");
}

TaskStatus MessageDispatcher::DispatchTask::run() {
    DEBUG_ENTER("run");
    m_dispatch->dispatch(m_req);
    DEBUG_LEAVE("run");
    return TaskStatus::Done;
}

dmgr::IDebug *MessageDispatcher::m_dbg = 0;
dmgr::IDebug *MessageDispatcher::DispatchTask::m_dbg = 0;

} /* namespace lls */
