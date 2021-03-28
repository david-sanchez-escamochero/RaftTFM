#pragma once


#include "IConnector.h"
#include "RPC.h"
#include "Communication.h"
#include "Server.h"
#include <queue>
#include <chrono>
#include <mutex>

using namespace std::chrono;

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
	void			check_if_there_is_candidate_or_leader();
	uint32_t		receiving_heartbeats_;	
	void			dispatch_append_request_vote(RPC *rpc);
	bool			i_have_already_voted_;
	milliseconds	last_time_stam_taken_miliseconds_;
	std::mutex		mu_follower_;
	uint32_t		count_check_if_there_is_candidate_or_leader_;
	std::thread		thread_check_candidate_;
};

