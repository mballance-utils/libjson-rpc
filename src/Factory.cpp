/*
 * Factory.cpp
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
#include "jrpc/FactoryExt.h"
#include "EventLoop.h"
#include "Factory.h"
#include "MessageRequestResponseStream.h"
#include "NBSocketServerMessageDispatcher.h"
#include "StdioMessageTransport.h"
#include "ReqMsg.h"
#include "RspMsg.h"
#include "Semaphore.h"
#include "TaskQueue.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>


namespace jrpc {


Factory::Factory() {

}

Factory::~Factory() {

}

IEventLoop *Factory::mkEventLoop() {
    return new EventLoop(m_dmgr);
}

std::pair<int32_t, int32_t> Factory::mkSocketServer() {
   // Create a server
    int server_fd, server_sock;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        return {-1, -1};
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    int res = bind(server_fd, (struct sockaddr *)&addr,
        sizeof(addr));
    if (res == -1) {
        return {-1, -1};
    }

    struct sockaddr_in sock_addr;
    socklen_t sock_addr_len = sizeof(sock_addr);
    getsockname(server_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);

    res = listen(server_fd, 1);
    if (res == -1) {
        return {-1, -1};
    }

    return {ntohs(sock_addr.sin_port), server_fd};
}

int32_t Factory::mkSocketClientConnection(int32_t port) {
    // Create a client
    struct sockaddr_in serv_addr;
    int client_fd;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(port);

    int res = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (res == -1) {
        return -1;
    }

    return client_fd;
}

int32_t Factory::mkSocketClientConnection(const std::string &skt) {
    // Create a client
    struct sockaddr_un serv_addr;
    int client_fd = -1;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        return -1;
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr_un));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, skt.c_str());

    int len = sizeof(serv_addr);

    int res = connect(client_fd, (struct sockaddr *)&serv_addr, len);

    if (res == -1) {
        return -1;
    }

    return client_fd;
}

IMessageRequestResponseStream *Factory::mkMessageRequestResponseStream(
        IEventLoop          *loop,
        int32_t             sock_fd) {
    return new MessageRequestResponseStream(m_dmgr, loop, sock_fd);
}

IMessageTransport *Factory::mkNBSocketMessageTransport(
        IEventLoop          *loop,
        int32_t             sock_fd) {
    return new NBSocketMessageTransport(m_dmgr, loop, sock_fd);
}

IMessageTransport *Factory::mkStdioMessageTransport(
        IEventLoop          *loop) {
    return new StdioMessageTransport(m_dmgr, loop);
}

IMessageDispatcher *Factory::mkNBSocketServerMessageDispatcher(
        ITaskQueue          *queue,
        IMessageTransport   *transport) {
    return new NBSocketServerMessageDispatcher(this, queue, transport);
}

IRspMsg *Factory::mkRspMsg(const nlohmann::json &msg) {
    return RspMsg::mk(msg);
}

IRspMsg *Factory::mkRspMsgSuccess(
        const std::string       &id,
        const nlohmann::json    &result) {
    return new RspMsg(id, result);
}

IRspMsg *Factory::mkRspMsgError(
        const std::string       &id,
        int32_t                 code,
        const std::string       &msg,
        const nlohmann::json    &data) {
    return new RspMsg(id, code, msg, data);
}

ITaskQueue *Factory::mkTaskQueue(
        ITaskScheduler          *sched) {
    return new TaskQueue(m_dmgr, sched);
}

ISemaphore *Factory::mkSemaphore(int32_t iv) {
    return new Semaphore(iv);
}

IFactory *Factory::inst() {
    if (!m_inst.get()) {
        m_inst = std::unique_ptr<IFactory>(new Factory());
    }
    return m_inst.get();
}

std::unique_ptr<IFactory> Factory::m_inst;

}

jrpc::IFactory *jrpc_getFactory() {
    return jrpc::Factory::inst();
}

