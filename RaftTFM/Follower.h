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
	void send(void*);
	void receive(RPC *rpc);
protected:
	Communication communication_;
	RPC rpc_;
	void* server_;
	uint32_t time_out_to_receive_heartbeat = TIME_OUT_TO_RECEIVE_HEART_BEAT;
	bool have_to_die_;
	std::queue<RPC> queue_;
	void dispatch(RPC* rpc);
};

