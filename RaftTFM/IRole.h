#pragma once

#include "RPC.h"
#include <string>


class IRole {

public:
	virtual void start()																							= 0;
	virtual void send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)	= 0;
	virtual void receive(RPC* rpc)																					= 0;
	virtual ~IRole() {}
	
};
