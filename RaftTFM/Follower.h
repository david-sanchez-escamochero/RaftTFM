#pragma once


#include "IConnector.h"
#include "RPC.h"
#include "Communication.h"
#include "Server.h"
#include <queue>

class Follower : public IConnector
{
public:
	Follower(void* server);
	~Follower();
	void send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void receive(RPC *rpc);
	void start();

protected:
	Communication	communication_;
	RPC				rpc_;
	void*			server_;	
	bool			have_to_die_;
	std::queue<RPC> queue_;
	void			dispatch(RPC* rpc);
	void			check_candidate();
	uint32_t		receiving_heartbeats_;	
	void			dispatch_append_request_vote(RPC *rpc);
	bool			i_have_already_voted;
};

