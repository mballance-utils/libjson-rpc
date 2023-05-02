/**
 * NBSocketServerMessageDispatcher.h
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
#include "MessageDispatcher.h"
#include "NBSocketMessageTransport.h"

namespace jrpc {



class NBSocketServerMessageDispatcher : 
    public virtual MessageDispatcher {
public:
    NBSocketServerMessageDispatcher(
        IFactory                *factory,
        IMessageTransport       *transport);

    virtual ~NBSocketServerMessageDispatcher();
    
    virtual IEventLoop *getLoop() override {
        return m_transport->getLoop();
    }

private:
    static dmgr::IDebug                 *m_dbg;
    IMessageTransport                   *m_transport;

};

}


