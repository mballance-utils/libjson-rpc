/*
 * TestBase.cpp
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
#include "TestBase.h"
#include "dmgr/FactoryExt.h"
#include "jrpc/FactoryExt.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

namespace jrpc {


TestBase::TestBase() {

}

TestBase::~TestBase() {

}

void TestBase::SetUp() {
    dmgr::IFactory *dmgr_factory = dmgr_getFactory();

    m_factory = jrpc_getFactory();
    m_factory->init(dmgr_factory->getDebugMgr());

    m_factory->getDebugMgr()->enable(false);
}

std::pair<int32_t, int32_t> TestBase::mkClientServerPair() {
    std::pair<int32_t, int32_t> srv_port_sock = m_factory->mkSocketServer();

    int client_fd = m_factory->mkSocketClientConnection(srv_port_sock.first);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    // Now, wait for a readable event
    int addrlen = sizeof(addr);
    int server_sock = accept(srv_port_sock.second, 
        (struct sockaddr *)&addr, (socklen_t *)&addrlen); 

    return {client_fd, server_sock};
}

TestBase::ReqRspDispatcherLoop TestBase::mkReqDispatcher() {
    IEventLoop *loop = m_factory->mkEventLoop();
    ITaskQueue *queue = m_factory->mkTaskQueue(loop);
    std::pair<int32_t, int32_t> client_srv_fd = mkClientServerPair();

    IMessageRequestResponseStream *reqrsp = m_factory->mkMessageRequestResponseStream(
        loop,
        client_srv_fd.first);
    IMessageTransport *srv_transport = m_factory->mkNBSocketMessageTransport(
        loop, client_srv_fd.second);
    IMessageDispatcher *dispatch = m_factory->mkNBSocketServerMessageDispatcher(
        queue, srv_transport);

    ReqRspDispatcherLoop ret;

    ret.reqrsp = reqrsp;
    ret.dispatch = dispatch;
    ret.loop = loop;
    ret.queue = queue;

    return ret;
}

void TestBase::enableDebug(bool en) {
    m_factory->getDebugMgr()->enable(en);
}

std::pair<jrpc::IMessageTransportUP, jrpc::IMessageTransportUP> TestBase::mkTransportPair(
    jrpc::IEventLoop    *loop) {

    std::pair<int32_t, int32_t> srv_port_sock = m_factory->mkSocketServer();

    int client_fd = m_factory->mkSocketClientConnection(srv_port_sock.first);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    // Now, wait for a readable event
    int addrlen = sizeof(addr);
    int server_sock = accept(srv_port_sock.second, 
        (struct sockaddr *)&addr, (socklen_t *)&addrlen); 

    return {
        jrpc::IMessageTransportUP(m_factory->mkNBSocketMessageTransport(loop, server_sock)),
        jrpc::IMessageTransportUP(m_factory->mkNBSocketMessageTransport(loop, client_fd))
    };
}

}
