/*
 * StdioMessageTransport.cpp
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
#include "StdioMessageTransport.h"
#include <unistd.h>


namespace jrpc {


StdioMessageTransport::StdioMessageTransport(
    dmgr::IDebugMgr         *dmgr,
    IEventLoop              *loop) : 
        BaseMessageTransport(dmgr, loop) {
    DEBUG_INIT("StdioMessageTransport", dmgr);
}

StdioMessageTransport::~StdioMessageTransport() {

}

void StdioMessageTransport::init(IMessageTransport *peer) {
    BaseMessageTransport::init(peer);

    // We expect to receive data, so register
    // an event callback with the event loop
    m_loop->addFdReadTask([this] { this->read_ev(); }, 0);
    DEBUG_LEAVE("init");
}

int32_t StdioMessageTransport::send_data(const char *data, int32_t sz) {
    DEBUG("send_data: %s (%d)", data, sz);
    ::write(1, data, sz);
}

void StdioMessageTransport::read_ev() {
	char tmp[1024];
	int32_t sz;
	int32_t ret = 0;

	DEBUG_ENTER("read_ev");

	// Poll for data
	sz = ::read(0, tmp, sizeof(tmp));

    if (sz > 0) {
        process_data(tmp, sz);

        // Re-register the event for more data
        m_loop->addFdReadTask([this] { this->read_ev(); }, 0);
    }

	DEBUG_LEAVE("read_ev %d", ret);
}

}
