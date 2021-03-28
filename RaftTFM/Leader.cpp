#include "Leader.h"
#include "Log.h"

Leader::Leader(void* server)
{	
	server_ = server;
	Log::trace("(Leader." + std::to_string(((Server*)server_)->get_server_id()) + ") I am a LEADER\r\n");
	
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

