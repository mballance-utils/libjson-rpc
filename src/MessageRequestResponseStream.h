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
#include "dmgr/IDebugMgr.h"
#include "jrpc/IMessageRequestResponseStream.h"

namespace jrpc {



class MessageRequestResponseStream : 
    public virtual IMessageRequestResponseStream {
public:
    MessageRequestResponseStream(
        dmgr::IDebugMgr         *dmgr,
        int32_t                 sock_fd
    );

    virtual ~MessageRequestResponseStream();

    virtual void setNotifyCallback(
        const std::function<void (const nlohmann::json &)> &func) override {
        m_notify_f = func;
    }

    virtual const nlohmann::json &invoke(const nlohmann::json &method) override;

    virtual void close() override;

private:
    static dmgr::IDebug                                 *m_dbg;
    int32_t                                             m_sock_fd;
    std::function<void (const nlohmann::json &)>        m_notify_f;

};

}


