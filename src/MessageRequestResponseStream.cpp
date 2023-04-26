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
#include <unistd.h>
#include "nlohmann/json.hpp"


namespace jrpc {


MessageRequestResponseStream::MessageRequestResponseStream(
    dmgr::IDebugMgr             *dmgr,
    int32_t                     sock_fd) : m_sock_fd(sock_fd), m_id(1) {
    DEBUG_INIT("MessageRequestResponseStream", dmgr);
}

MessageRequestResponseStream::~MessageRequestResponseStream() {

}

const nlohmann::json &MessageRequestResponseStream::invoke(
    const std::string       &method,
    const nlohmann::json    &params) {
    int32_t id = m_id;
    m_id++;

    nlohmann::json msg;
    msg["jsonrpc"] = "2.0";
    msg["id"] = id;
    msg["method"] = method;
    msg["params"] = params;

    std::string body = msg.dump();
    char tmp[64];
    sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

    ::send(m_sock_fd, tmp, strlen(tmp), 0);
    ::send(m_sock_fd, body.c_str(), body.size(), 0);

    // TODO: poll, waiting for a response
}

void MessageRequestResponseStream::close() {
    ::close(m_sock_fd);
}

dmgr::IDebug *MessageRequestResponseStream::m_dbg = 0;

}
