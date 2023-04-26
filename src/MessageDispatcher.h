/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * MessageDispatcher.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include <functional>
#include <map>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IMessageDispatcher.h"
#include "jrpc/IMessageTransport.h"

namespace jrpc {

class MessageDispatcher : public virtual IMessageDispatcher {
public:
	MessageDispatcher(dmgr::IDebugMgr *dmgr);

	virtual ~MessageDispatcher();

    virtual void init(
        IEventLoop                  *loop,
        IMessageTransport           *peer) override;

    virtual void registerMethod(
        const std::string                           &method,
        std::function<IRspMsgUP(IReqMsgUP &)> impl) override;

	/**
	 * Inbound message
	 */
	virtual void send(const nlohmann::json &msg) override;

private:
    static dmgr::IDebug                 *m_dbg;
    IEventLoop                          *m_loop;
    IMessageTransport                   *m_peer;
	std::map<std::string,std::function<IRspMsgUP(IReqMsgUP &)>> m_method_m;
};

} /* namespace lls */

