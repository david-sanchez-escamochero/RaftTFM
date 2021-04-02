#pragma once

#include "RaftDefs.h"
#include <stdint.h>
#include "IConnector.h"
#include "Server.h"
#include <mutex>


class Leader : public IConnector
{


public: 
	Leader(void* server);
	~Leader();

	void send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void receive(RPC* rpc);
	void start();
	

protected: 
	//  Volatile state on leaders 
	// (Reinitialized after election)
	uint32_t		next_index_[NUM_SERVERS];	// For each server, index of the next log entry	to send to that server(initialized to leader last log index + 1)
	uint32_t		match_index_[NUM_SERVERS];	// For each server, index of highest log entry known to be replicated on server	(initialized to 0, increases monotonically)
	void*			server_;
	std::mutex		mu_leader_;
	bool			have_to_die_;
	void			dispatch(RPC* rpc);
	void			dispatch_append_entry(RPC* rpc);
	void			dispatch_request_vote(RPC* rpc);
	void			dispatch_append_heart_beat(RPC* rpc);
};

