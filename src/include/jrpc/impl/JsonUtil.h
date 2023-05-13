/**
 * JsonUtil.h
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
#include <stdint.h>
#include "nlohmann/json.hpp"

namespace jrpc {



class JsonUtil {
public:
    JsonUtil() { }

    virtual ~JsonUtil() { }

    static int64_t getInt(const nlohmann::json &v) {
        int64_t ret = -1;
        if (v.is_number_integer()) {
            if (v.is_number_unsigned()) {
                ret = v.get<uint64_t>();
            } else {
                ret = v.get<int64_t>();
            }
        } else if (v.is_string()) {
            ret = strtoll(v.get<std::string>().c_str(), 0, 0);
        }

        return ret;
    }

    static int64_t getUInt(const nlohmann::json &v) {
        if (v.is_number_integer()) {
            if (v.is_number_unsigned()) {
                return v.get<uint64_t>();
            } else {
                return v.get<int64_t>();
            }
        } else if (v.is_string()) {
            return strtoull(v.get<std::string>().c_str(), 0, 0);
        }
    }

};

}


