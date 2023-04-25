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
 * IMessageTransport.h
 *
 *  Created on: Sep 21, 2020
 *      Author: ballance
 */

#pragma once
#include <string>
#include "jrpc/IEventLoop.h"
#include "nlohmann/json_fwd.hpp"

namespace jrpc {

class IMessageTransport {
public:
	virtual ~IMessageTransport() { }

    /**
     * Initialize the transport with its peer
     */
    virtual void init(
        IEventLoop          *loop,
        IMessageTransport   *peer) = 0;

    /**
     * Send an outbound message via this transport
     */
	virtual void send(const nlohmann::json &msg) = 0;

};

}

