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
    int32_t                     sock_fd) : 
	    m_msgbuf(0), m_msgbuf_idx(0), m_msgbuf_max(0),
        m_msg_state(0), m_msg_length(0),
    	m_sock_fd(sock_fd), m_loop(0), m_peer(0) {

}

NBSocketMessageTransport::~NBSocketMessageTransport() {
	if (m_msgbuf) {
		delete [] m_msgbuf;
	}
}

void NBSocketMessageTransport::init(
    IEventLoop              *loop,
    IMessageTransport       *peer) {
    m_loop = loop;
	m_peer = peer;

    // We expect to receive data, so register
    // an event callback with the event loop
    m_loop->addFdReadTask([this] { this->read_ev(); }, m_sock_fd);
}

void NBSocketMessageTransport::send(const nlohmann::json &msg) {
	char tmp[64];
	std::string body = msg.dump();
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	::send(m_sock_fd, tmp, strlen(tmp), 0);
	::send(m_sock_fd, body.c_str(), body.size(), 0);
}

void NBSocketMessageTransport::read_ev() {
	char tmp[1024];
	int32_t sz;
	int32_t ret = 0;

	DEBUG_ENTER("process");

	// Poll for data
	sz = ::recv(m_sock_fd, tmp, 1024, 0);

    if (sz > 0) {

		// Process data
		for (int32_t i=0; i<sz; i++) {
			switch (m_msg_state) {
			case 0: { // Waiting for a header
				if (tmp[i] == HEADER_PREFIX.at(m_msgbuf_idx)) {
					m_msgbuf_idx++;
				} else {
					m_msgbuf_idx = 0;
				}
				if (m_msgbuf_idx == HEADER_PREFIX.size()) {
					m_msgbuf_idx = 0;
					m_msg_state = 1;
				}
			} break;

			case 1: { // Collecting length up to first '\n'
				if (m_msgbuf_idx == 0 && isspace(tmp[i])) {
					// Skip leading whitespace
				} else {
					DEBUG("State 1: append %c", tmp[i]);
					msgbuf_append(tmp[i]);
					if (isspace(tmp[i])) {
						msgbuf_append(0);
						DEBUG("header=%s", m_msgbuf);
						m_msg_length = strtoul(m_msgbuf, 0, 10);
						DEBUG("len=%d", m_msg_length);
						// Reset the buffer to collect the payload
						m_msgbuf_idx = 0;
						m_msg_state = 2;
					}
				}

			} break;

			case 2: { // Collecting body data
				if (m_msgbuf_idx == 0 && isspace(tmp[i])) {
					// Skip leading whitespace
				} else {
					msgbuf_append(tmp[i]);
					if (m_msgbuf_idx >= m_msg_length) {
						msgbuf_append(0);
						DEBUG("Received message: \"%s\"", m_msgbuf);
						nlohmann::json msg;
						try {
							msg = nlohmann::json::parse(m_msgbuf);
							m_peer->send(msg);
						} catch (const std::exception &e) {
							fprintf(stdout, "Failed to parse msg \"%s\" %s\n",
									m_msgbuf, e.what());
						}
						m_msg_state = 0;
						m_msgbuf_idx = 0;
					}
				}
			} break;

			default: {
				m_msgbuf_idx = 0;
				m_msg_state = 0;
			}
			}
		}

        // Re-register the event for more data
        m_loop->addFdReadTask([this] { this->read_ev(); }, m_sock_fd);
    }

	DEBUG_LEAVE("read_ev %d", ret);
}

void NBSocketMessageTransport::msgbuf_resize_append(char c) {
	// Confirm that we need to resize
	if (m_msgbuf_idx+1 >= m_msgbuf_max) {
		// yup, must resize
		char *tmp = m_msgbuf;
		m_msgbuf = new char[m_msgbuf_max+1024];

		memcpy(m_msgbuf, tmp, m_msgbuf_idx);
		m_msgbuf_max += 1024;

		delete [] tmp;
	}

	m_msgbuf[m_msgbuf_idx++] = c;
}


const std::string NBSocketMessageTransport::HEADER_PREFIX = "Content-Length: ";
dmgr::IDebug *NBSocketMessageTransport::m_dbg = 0;

}
