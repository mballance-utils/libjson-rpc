/**
 * RspMsg.h
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
#include <string>
#include "jrpc/IRspMsg.h"
#include "nlohmann/json.hpp"

namespace jrpc {


class RspMsg : public virtual IRspMsg {
public:
    RspMsg(
        const std::string       &id,
        const nlohmann::json    &result);

    RspMsg(
        const std::string       &id,
        int32_t                 code,
        const std::string       &msg,
        const nlohmann::json    &result);

    virtual ~RspMsg();

    virtual const std::string &getId() const { 
        return m_id; 
    }

    /**
     * @brief Returns the result on success or error data on failure
     */
    virtual const nlohmann::json &getResult() {
        return m_result;
    }

    virtual int32_t getErrorCode() {
        return m_err_code;
    }

    virtual const std::string &getErrorMsg() {
        return m_msg;
    }

    static RspMsg *mk(const nlohmann::json &msg);

private:
    std::string                 m_id;
    int32_t                     m_err_code;
    std::string                 m_msg;
    nlohmann::json              m_result;
};

}


