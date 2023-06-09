/**
 * MessageRequestResponseStream.h
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
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IEventLoop.h"
#include "jrpc/IMessageRequestResponseStream.h"
#include "NBSocketMessageTransport.h"
#include "nlohmann/json.hpp"

namespace jrpc {



class MessageRequestResponseStream : 
    public virtual IMessageRequestResponseStream,
    public virtual IMessageTransport {
public:
    MessageRequestResponseStream(
        dmgr::IDebugMgr         *dmgr,
        IEventLoop              *loop,
        int32_t                 sock_fd
    );

    virtual ~MessageRequestResponseStream();

    virtual void setNotifyCallback(
        const std::function<void (const nlohmann::json &)> &func) override {
        m_notify_f = func;
    }

    virtual IRspMsgUP invoke(
        const std::string       &method,
        const nlohmann::json    &params) override;

    virtual void close() override;

    virtual void init(IMessageTransport *peer) override { }

	virtual void send(const nlohmann::json &msg) override;

    virtual IEventLoop *getLoop() override {
        return m_loop;
    }

private:
    static dmgr::IDebug                                 *m_dbg;
    NBSocketMessageTransport                            m_transport;
    IEventLoop                                          *m_loop;
    std::function<void (const nlohmann::json &)>        m_notify_f;
    int32_t                                             m_id;
    std::vector<nlohmann::json>                         m_rsp;

};

}


