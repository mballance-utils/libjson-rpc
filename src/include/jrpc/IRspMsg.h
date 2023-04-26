/**
 * IRspMsg.h
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
#include <memory>
#include <string>
#include "nlohmann/json_fwd.hpp"

namespace jrpc {

class IRspMsg;
using IRspMsgUP=std::unique_ptr<IRspMsg>;
class IRspMsg {
public:

    virtual ~IRspMsg() { }

    virtual int32_t getId() = 0;

    /**
     * @brief Returns the result on success or error data on failure
     */
    virtual const nlohmann::json &getResult() = 0;

    virtual int32_t getErrorCode() = 0;

    virtual const std::string &getErrorMsg() = 0;

};

} /* namespace jrpc */


