#pragma once

#include "IConnector.h"
#include "Server.h"
#include <string>

class Candidate : public IConnector
{
public:
	Candidate(void *server);
	~Candidate();
	void send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void receive(RPC* rpc);
	void start();
protected:
	void* server_;
	void send_request_vote_to_all_servers();
	bool there_is_not_leader_;
	bool have_to_die_;
	void reset_receive_votes();
	uint32_t receive_votes_;
};

