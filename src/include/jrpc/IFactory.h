/**
 * IFactory.h
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
#include "jrpc/IEventLoop.h"
#include "jrpc/IMessageDispatcher.h"
#include "jrpc/IMessageRequestResponseStream.h"
#include "jrpc/IRspMsg.h"
#include "jrpc/ISemaphore.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/ITaskScheduler.h"

namespace jrpc {



class IFactory {
public:

    virtual ~IFactory() { }

    virtual void init(dmgr::IDebugMgr *dmgr) = 0;

    virtual dmgr::IDebugMgr *getDebugMgr() = 0;

    virtual IEventLoop *mkEventLoop() = 0;

    virtual std::pair<int32_t, int32_t> mkSocketServer(int32_t port=-1) = 0;

    virtual int32_t mkSocketClientConnection(int32_t port) = 0;

    virtual int32_t mkSocketClientConnection(const std::string &skt) = 0;

    virtual IMessageRequestResponseStream *mkMessageRequestResponseStream(
        IEventLoop          *loop,
        int32_t             sock_fd) = 0;

    virtual IMessageTransport *mkNBSocketMessageTransport(
        IEventLoop          *loop,
        int32_t             sock_fd) = 0;

    virtual IMessageTransport *mkStdioMessageTransport(
        IEventLoop          *loop) = 0;

    virtual IMessageDispatcher *mkNBSocketServerMessageDispatcher(
        ITaskQueue              *queue,
        IMessageTransport       *transport) = 0;

    virtual IRspMsg *mkRspMsg(
        const nlohmann::json    &msg) = 0;

    virtual IRspMsg *mkRspMsgSuccess(
        const std::string       &id,
        const nlohmann::json    &result) = 0;

    virtual IRspMsg *mkRspMsgError(
        const std::string       &id,
        int32_t                 code,
        const std::string       &msg,
        const nlohmann::json    &data) = 0;

    virtual ITaskQueue *mkTaskQueue(
        ITaskScheduler          *sched) = 0;

    virtual ISemaphore *mkSemaphore(int32_t iv=0) = 0;

};

} /* namespace jrpc */


