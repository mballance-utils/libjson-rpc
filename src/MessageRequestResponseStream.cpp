/*
 * MessageRequestResponseStream.cpp
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
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#else
#include <winsock.h>
#endif
#include "dmgr/impl/DebugMacros.h"
#include "MessageRequestResponseStream.h"
#include "RspMsg.h"
#include <unistd.h>
#include "nlohmann/json.hpp"


namespace jrpc {


MessageRequestResponseStream::MessageRequestResponseStream(
    dmgr::IDebugMgr             *dmgr,
    IEventLoop                  *loop,
    int32_t                     sock_fd) : 
        m_transport(dmgr, sock_fd), m_loop(loop), m_id(1) {
    DEBUG_INIT("MessageRequestResponseStream", dmgr);

    m_transport.init(loop, this);
}

MessageRequestResponseStream::~MessageRequestResponseStream() {

}

IRspMsgUP MessageRequestResponseStream::invoke(
    const std::string       &method,
    const nlohmann::json    &params) {
    DEBUG_ENTER("invoke %s", method.c_str());
    int32_t id = m_id;
    m_id++;

    nlohmann::json msg;
    msg["jsonrpc"] = "2.0";
    msg["id"] = id;
    msg["method"] = method;
    msg["params"] = params;

    m_transport.send(msg);

    m_rsp.clear();
    do {
        if (!m_loop->process_one_event(-1)) {
            break;
        }
    } while (!m_rsp.size());

    if (m_rsp.size()) {
        // Decode response and create a proper message
        nlohmann::json &rsp = m_rsp.front();

        if (rsp.find("error") != rsp.end()) {
            // Error response
            fprintf(stdout, "Error response\n");
            DEBUG_LEAVE("invoke %s", method.c_str());
        } else {
            // Success response
            DEBUG_LEAVE("invoke %s", method.c_str());
            return IRspMsgUP(new RspMsg(id, rsp["result"]));
        }
    } else {
        DEBUG_LEAVE("invoke %s (no message)", method.c_str());
        return IRspMsgUP(); // No message
    }
}

void MessageRequestResponseStream::close() {
//    ::close(m_sock_fd);
}

void MessageRequestResponseStream::send(const nlohmann::json &msg) {
    DEBUG_ENTER("send");
    m_rsp.push_back(msg);
    DEBUG_LEAVE("send");
}

dmgr::IDebug *MessageRequestResponseStream::m_dbg = 0;

}
