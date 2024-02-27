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
#include "jrpc/IFactory.h"
#include "jrpc/IMessageDispatcher.h"
#include "jrpc/IMessageTransport.h"
#include "jrpc/ITask.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/impl/TaskBase.h"

namespace jrpc {

class MessageDispatcher : public virtual IMessageDispatcher {
public:
	MessageDispatcher(
        IFactory        *factory,
        ITaskQueue      *queue);

	virtual ~MessageDispatcher();

    virtual void init(IMessageTransport *peer) override;

    virtual void setResponseHandler(
        const std::function<void(const std::string &,IRspMsgUP &)> &handler) override {
        m_handler = handler;
    }

    virtual void registerMethod(
        const std::string               &method,
        IMessageDispatcher::MethodF     method_f) override;

    virtual IMessageTransport *getPeer() override {
        return m_peer;
    }

	/**
	 * Inbound message
	 */
	virtual void send(const nlohmann::json &msg) override;

private:

    void dispatch(IReqMsgUP &req);

    class DispatchTask : public TaskBase {
    public:
        DispatchTask(
            ITaskGroup          *group,
            MessageDispatcher   *dispatch, 
            IReqMsgUP           &req) :
            TaskBase(group), m_dispatch(dispatch), m_req(std::move(req)) {
        }
        virtual ~DispatchTask() { }

        virtual TaskStatus run() override;

        virtual ITask *clone() override { return 0; }

        static dmgr::IDebug             *m_dbg;
        MessageDispatcher               *m_dispatch;
        IReqMsgUP                       m_req;
    };

private:
    static dmgr::IDebug                 *m_dbg;
    IFactory                            *m_factory;
    ITaskQueue                          *m_queue;
    IMessageTransport                   *m_peer;
	std::map<std::string,IMessageDispatcher::MethodF>       m_method_m;
    std::function<void(const std::string &,IRspMsgUP &)>    m_handler;
};

} /* namespace lls */

