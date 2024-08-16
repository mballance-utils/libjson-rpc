/*
 * BaseMessageTransport.cpp
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
#include "nlohmann/json.hpp"
#include "BaseMessageTransport.h"


namespace jrpc {


BaseMessageTransport::BaseMessageTransport(
    dmgr::IDebugMgr     *dmgr,
    IEventLoop          *loop) :
    m_dmgr(dmgr), m_loop(loop), m_msgbuf(0), m_msg_state(0), 
    m_msg_length(0), m_peer(0) { 

	m_msgbuf_idx = 0;
	m_msgbuf_max = 1024;
	m_msgbuf = new char[m_msgbuf_max];
}

BaseMessageTransport::~BaseMessageTransport() {
	if (m_msgbuf) {
		delete [] m_msgbuf;
	}
}

void BaseMessageTransport::init(IMessageTransport *peer) {
	m_peer = peer;

    // We expect to receive data, so register
    // an event callback with the event loop
}

void BaseMessageTransport::send(const nlohmann::json &msg) {
    DEBUG_ENTER("send");
	char tmp[64];
	std::string body = msg.dump();
    char *msg_b = new char[64+body.size()];
	sprintf(msg_b, "Content-Length: %d\r\n\r\n", body.size());
    strcat(msg_b, body.c_str());

    DEBUG("Message: %s", msg_b);

	send_data(msg_b, strlen(msg_b));
//	send_data(body.c_str(), body.size());
    DEBUG_LEAVE("send");
}


int32_t BaseMessageTransport::process_data(const char *data, int32_t sz) {
	int32_t ret = 0;

	DEBUG_ENTER("process_data sz=%d", sz);

	// Process data
	for (int32_t i=0; i<sz; i++) {
		switch (m_msg_state) {
        // Detect header 
		case 0: { // Processing headers
			if (data[i] == HEADER_PREFIX.at(m_msgbuf_idx)) {
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
			if (m_msgbuf_idx == 0 && isspace(data[i])) {
				// Skip leading whitespace
			} else {
				DEBUG("State 1: append %c", data[i]);
				msgbuf_append(data[i]);
				if (isspace(data[i])) {
					msgbuf_append(0);
					DEBUG("header=%s", m_msgbuf);
					m_msg_length = strtoul(m_msgbuf, 0, 10);
					DEBUG("len=%d", m_msg_length);
					// Reset the buffer to collect the payload
					m_msgbuf_idx = 0;
					m_msg_state = 2;
                    m_nl_count = (data[i] == '\n')?1:0;
				}
			}
		} break;

        case 2: { // Skip the rest of the header.
                  // This means reading until we have two back-to-back newlines
            if (data[i] == '\n') {
                m_nl_count++;
            } else if (data[i] != '\r') {
                m_nl_count = 0;
            }
            if (m_nl_count == 2) {
                m_msg_state = 3;
            }
        } break;

		case 3: { // Collecting body data
			if (m_msgbuf_idx == 0 && isspace(data[i])) {
				// Skip leading whitespace
			} else {
				msgbuf_append(data[i]);
				if (m_msgbuf_idx >= m_msg_length) {
					msgbuf_append(0);
					DEBUG("Received message: \"%s\"", m_msgbuf);
					nlohmann::json msg;
					try {
						msg = nlohmann::json::parse(m_msgbuf);
					} catch (const std::exception &e) {
                        DEBUG_ERROR("Failed to parse msg \"%s\" %s\n",
                            m_msgbuf, e.what());
						fprintf(stderr, "Failed to parse msg \"%s\" %s\n",
								m_msgbuf, e.what());
					}
					try {
						m_peer->send(msg);
					} catch (const std::exception &e) {
                        DEBUG_ERROR("Failed to send msg \"%s\" %s\n",
                            m_msgbuf, e.what());
					}
					m_msg_state = 0;
					m_msgbuf_idx = 0;
                    m_msg_length = -1;
                    ret = 1; // processed a message
				}
			}
		} break;

		default: {
			m_msgbuf_idx = 0;
			m_msg_state = 0;
		}
		}
	}

	DEBUG_LEAVE("process_data %d", ret);
	return ret;
}

void BaseMessageTransport::msgbuf_resize_append(char c) {
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

const std::string	BaseMessageTransport::HEADER_PREFIX = "Content-Length: ";
const char *BaseMessageTransport::HEADER_PREF_CONTENT = "Content-";
const char *BaseMessageTransport::HEADER_PREF_HOST = "Host";
const char *BaseMessageTransport::HEADER_PREF_ACCEPT = "Accept";
dmgr::IDebug *BaseMessageTransport::m_dbg = 0;

}
