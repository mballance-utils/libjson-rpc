/**
 * IMessageDispatcher.h
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
#include <functional>
#include <memory>
#include "jrpc/IReqMsg.h"
#include "jrpc/IRspMsg.h"
#include "jrpc/IMessageTransport.h"

namespace jrpc {

class IMessageDispatcher;
using IMessageDispatcherUP=std::unique_ptr<IMessageDispatcher>;
class IMessageDispatcher : public virtual IMessageTransport {
public:
    using MethodF=std::function<void(IReqMsgUP &)>;
public:

    virtual ~IMessageDispatcher() { }

    virtual void setResponseHandler(
        const std::function<void(const std::string &,IRspMsgUP &)> &handler) = 0;

    virtual void registerMethod(
        const std::string       &method,
        MethodF                 method_f) = 0;

    virtual IMessageTransport *getPeer() = 0;

};

} /* namespace jrpc */


