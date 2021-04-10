#pragma once

#include "IRole.h"
#include "Server.h"
#include <string>
#include <mutex>



class Candidate : public IRole
{
public:
	Candidate(void *server);
	~Candidate();
	void send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void receive(RPC* rpc);
	void start();

	
protected:
	void*		server_;
	void		send_request_vote_to_all_servers();
	bool		there_is_leader_;
	bool		have_to_die_;
	void		reset_receive_votes();
	uint32_t	received_votes_;
	std::mutex	mu_candidate_;


	std::thread thread_send_request_vote_to_all_servers_;

	// Dispatchers.
	void dispatch(RPC *rpc);
	void dispatch_append_entry(RPC* rpc);
	void dispatch_request_vote(RPC* rpc);
	void dispatch_append_heart_beat(RPC* rpc);
	void dispatch_client_request_leader(RPC* rpc);
	void dispatch_client_request_value(RPC* rpc);



	std::condition_variable cv_send_request_vote_to_all_servers_;

	
};

