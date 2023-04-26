/**
 * ReqMsg.h
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
#include "jrpc/IReqMsg.h"
#include "nlohmann/json.hpp"

namespace jrpc {


class ReqMsg : public virtual IReqMsg {
public:
    ReqMsg(
        int32_t                 id,
        const std::string       &method,
        const nlohmann::json    &params);

    virtual ~ReqMsg();

    virtual int32_t getId() override {
        return m_id;
    }

    virtual const std::string &getMethod() override {
        return m_method;
    }

    virtual const nlohmann::json &getParams() override {
        return m_params;
    }

private:
    int32_t                     m_id;
    std::string                 m_method;
    nlohmann::json              m_params;

};

}


