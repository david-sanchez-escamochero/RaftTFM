#pragma once

#include "RPC.h"


class IConnector {

public:
	virtual void send(void*) = 0;
	virtual void receive(RPC* rpc) = 0;
};
