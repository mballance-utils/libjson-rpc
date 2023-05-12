/**
 * StdioMessageTransport.h
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
#include "BaseMessageTransport.h"

namespace jrpc {



class StdioMessageTransport : public virtual BaseMessageTransport {
public:
    StdioMessageTransport(
        dmgr::IDebugMgr         *dmgr,
        IEventLoop              *loop);

    virtual ~StdioMessageTransport();

	virtual void init(IMessageTransport *peer) override;

protected:

    virtual int32_t send_data(const char *data, int32_t sz) override;

private:
    void read_ev();

};

}

