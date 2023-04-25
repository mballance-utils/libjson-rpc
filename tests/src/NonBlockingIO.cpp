/*
 * NonBlockingIO.cpp
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
#include "NonBlockingIO.h"
#include "jrpc/IEventLoop.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


namespace jrpc {


NonBlockingIO::NonBlockingIO() {

}

NonBlockingIO::~NonBlockingIO() {

}

TEST_F(NonBlockingIO, srv_accept) {
    IEventLoopUP loop(m_factory->mkEventLoop());

    // Create a server
    int server_fd, server_sock;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(server_fd, -1);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    int res = bind(server_fd, (struct sockaddr *)&addr,
        sizeof(addr));
    ASSERT_NE(res, -1);

    struct sockaddr_in sock_addr;
    socklen_t sock_addr_len = sizeof(sock_addr);
    getsockname(server_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);

    res = listen(server_fd, 1);
    ASSERT_NE(res, -1);

    // Now, wait for a readable event
    int addrlen = sizeof(addr);
    server_sock = -1;
    bool accept_event_called = false;
    loop->addFdReadTask([&] {
        fprintf(stdout, "Accept event\n");
        accept_event_called = true;
        server_sock = accept(server_fd, 
        (struct sockaddr *)&addr, (socklen_t *)&addrlen); 
        }, server_fd);

    // Create a client
    struct sockaddr_in serv_addr;
    int client_fd;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = sock_addr.sin_port;

    res = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    ASSERT_NE(res, -1);

    for (uint32_t i=0; i<4 && !accept_event_called; i++) {
        loop->process_one_event(0);
    }

    ASSERT_TRUE(accept_event_called);
    ASSERT_NE(server_sock, -1);

    close(server_sock);
    close(client_fd);
    close(server_fd);
}

TEST_F(NonBlockingIO, srv_accept_2) {
    IEventLoopUP loop(m_factory->mkEventLoop());

    std::pair<int32_t, int32_t> srv_port_sock = m_factory->mkSocketServer();
    ASSERT_NE(srv_port_sock.first, -1);
    ASSERT_NE(srv_port_sock.second, -1);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    // Now, wait for a readable event
    int addrlen = sizeof(addr);
    int server_sock = -1;
    bool accept_event_called = false;
    loop->addFdReadTask([&] {
        fprintf(stdout, "Accept event\n");
        accept_event_called = true;
        server_sock = accept(srv_port_sock.second, 
        (struct sockaddr *)&addr, (socklen_t *)&addrlen); 
        }, srv_port_sock.second);

    int client_fd = m_factory->mkSocketClientConnection(srv_port_sock.first);
    ASSERT_NE(client_fd, -1);

    for (uint32_t i=0; i<4 && !accept_event_called; i++) {
        loop->process_one_event(0);
    }

    ASSERT_TRUE(accept_event_called);
    ASSERT_NE(server_sock, -1);

    close(server_sock);
    close(client_fd);
    close(srv_port_sock.second);
}

TEST_F(NonBlockingIO, srv_accept_3) {
    IEventLoopUP loop(m_factory->mkEventLoop());

    std::pair<int32_t, int32_t> srv_port_sock = m_factory->mkSocketServer();
    ASSERT_NE(srv_port_sock.first, -1);
    ASSERT_NE(srv_port_sock.second, -1);

    int client_fd = m_factory->mkSocketClientConnection(srv_port_sock.first);
    ASSERT_NE(client_fd, -1);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; // pick any port

    // Now, wait for a readable event
    int addrlen = sizeof(addr);
    int server_sock = accept(srv_port_sock.second, 
        (struct sockaddr *)&addr, (socklen_t *)&addrlen); 

    ASSERT_NE(server_sock, -1);

    close(server_sock);
    close(client_fd);
    close(srv_port_sock.second);
}

}
