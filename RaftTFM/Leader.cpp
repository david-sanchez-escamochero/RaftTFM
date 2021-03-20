#include "Leader.h"

Leader::Leader(void* server)
{
	server_ = server;
}

Leader::~Leader()
{
}

void Leader::start()
{
}

void Leader::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
}

void Leader::receive(RPC* rpc)
{
	
}

