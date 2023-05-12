/*
 * NBSocketMessageTransport.cpp
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
#include "nlohmann/json.hpp"
#include "NBSocketMessageTransport.h"


namespace jrpc {


NBSocketMessageTransport::NBSocketMessageTransport(
    dmgr::IDebugMgr             *dmgr,
    IEventLoop                  *loop,
    int32_t                     sock_fd) : 
        BaseMessageTransport(dmgr, loop), m_sock_fd(sock_fd) {
    DEBUG_INIT("MBSocketMessageTransport", dmgr);
}

NBSocketMessageTransport::~NBSocketMessageTransport() {

}

void NBSocketMessageTransport::init(IMessageTransport *peer) {
    DEBUG_ENTER("init");
    BaseMessageTransport::init(peer);

    // We expect to receive data, so register
    // an event callback with the event loop
    m_loop->addFdReadTask([this] { this->read_ev(); }, m_sock_fd);
    DEBUG_LEAVE("init");
}

int32_t NBSocketMessageTransport::send_data(const char *data, int32_t sz) {
    return ::send(m_sock_fd, data, sz, 0);
}

void NBSocketMessageTransport::read_ev() {
	char tmp[1024];
	int32_t sz;
	int32_t ret = 0;

	DEBUG_ENTER("read_ev");

	// Poll for data
	sz = ::recv(m_sock_fd, tmp, 1024, 0);

    if (sz > 0) {
        process_data(tmp, sz);

        // Re-register the event for more data
        m_loop->addFdReadTask([this] { this->read_ev(); }, m_sock_fd);
    }

	DEBUG_LEAVE("read_ev %d", ret);
}

}
