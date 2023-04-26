/*
 * NBSocketServerMessageDispatcher.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "NBSocketServerMessageDispatcher.h"


namespace jrpc {


NBSocketServerMessageDispatcher::NBSocketServerMessageDispatcher(
    dmgr::IDebugMgr             *dmgr,
    IEventLoop                  *loop,
    int32_t                     sock_fd) : 
        MessageDispatcher(dmgr), m_transport(dmgr, sock_fd) {
    DEBUG_INIT("NBSocketServerMessageDispatcher", dmgr);

    m_transport.init(loop, this);
    init(loop, &m_transport);
}

NBSocketServerMessageDispatcher::~NBSocketServerMessageDispatcher() {

}

dmgr::IDebug *NBSocketServerMessageDispatcher::m_dbg = 0;

}