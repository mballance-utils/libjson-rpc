/**
 * NBSocketMessageTransport.h
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
#pragma once
#include <string>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IEventLoop.h"
#include "jrpc/IMessageTransport.h"

namespace jrpc {

class NBSocketMessageTransport : public IMessageTransport {
public:
    NBSocketMessageTransport(
        dmgr::IDebugMgr         *dmgr,
        IEventLoop              *loop,
        int32_t                 sock_fd);

    virtual ~NBSocketMessageTransport();

	virtual void init(IMessageTransport       *peer) override;

	virtual void send(const nlohmann::json &msg) override;

    virtual IEventLoop *getLoop() override {
        return m_loop;
    }

private:
    void read_ev();

	void msgbuf_append(char c) {
		if (m_msgbuf_idx+1 < m_msgbuf_max) {
			m_msgbuf[m_msgbuf_idx++] = c;
		} else {
			msgbuf_resize_append(c);
		}
	}

	void msgbuf_resize_append(char c);

private:
    IEventLoop                  *m_loop;
	char						*m_msgbuf;
	uint32_t					m_msgbuf_idx;
	uint32_t					m_msgbuf_max;
	uint32_t					m_msg_state;
	uint32_t					m_msg_length;
	int32_t						m_sock_fd;
	IMessageTransport			*m_peer;

    static dmgr::IDebug         *m_dbg;
	static const std::string	HEADER_PREFIX;


};

}


