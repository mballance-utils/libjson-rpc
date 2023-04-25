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
#include "dmgr/impl/DebugMacros.h"
#include "MessageRequestResponseStream.h"
#include <unistd.h>


namespace jrpc {


MessageRequestResponseStream::MessageRequestResponseStream(
    dmgr::IDebugMgr             *dmgr,
    int32_t                     sock_fd) : m_sock_fd(sock_fd) {
    DEBUG_INIT("MessageRequestResponseStream", dmgr);
}

MessageRequestResponseStream::~MessageRequestResponseStream() {

}

const nlohmann::json &MessageRequestResponseStream::invoke(
    const nlohmann::json &method) {

}

void MessageRequestResponseStream::close() {
    ::close(m_sock_fd);
}

dmgr::IDebug *MessageRequestResponseStream::m_dbg = 0;

}
