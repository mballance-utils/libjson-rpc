/**
 * Factory.h
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
#include <memory>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IFactory.h"

namespace jrpc {



class Factory : public virtual IFactory {
public:
    Factory();

    virtual ~Factory();

    virtual void init(dmgr::IDebugMgr *dmgr) override {
        m_dmgr = dmgr;
    }

    virtual dmgr::IDebugMgr *getDebugMgr() override {
        return m_dmgr;
    }

    virtual IEventLoop *mkEventLoop() override;
    
    virtual std::pair<int32_t, int32_t> mkSocketServer() override;

    virtual int32_t mkSocketClientConnection(int32_t port) override;

    virtual IMessageRequestResponseStream *mkMessageRequestResponseStream(
        IEventLoop          *loop,
        int32_t             sock_fd) override;

    virtual IMessageDispatcher *mkNBSocketServerMessageDispatcher(
        IEventLoop          *loop,
        int32_t             sock_fd) override;

    virtual IRspMsg *mkRspMsgSuccess(
        int32_t                 id,
        const nlohmann::json    &result) override;

    virtual IRspMsg *mkRspMsgError(
        int32_t                 id,
        int32_t                 code,
        const std::string       &msg,
        const nlohmann::json    &data) override;

    static IFactory *inst();

private:
    dmgr::IDebugMgr                         *m_dmgr;
    static std::unique_ptr<IFactory>        m_inst;

};

}


