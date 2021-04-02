#include "Leader.h"
#include "Log.h"

Leader::Leader(void* server)
{	
	server_ = server;
	Log::trace("(Leader." + std::to_string(((Server*)server_)->get_server_id()) + ") I am a LEADER\r\n");

	have_to_die_ = false;	
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
	dispatch(rpc);
}


void Leader::dispatch_append_entry(RPC* rpc)
{
	if (rpc->rpc_direction == RPCDirection::rpc_in_invoke) {
	}
	else if (rpc->rpc_direction == RPCDirection::rpc_out_result) {
	}
}

void Leader::dispatch_request_vote(RPC* rpc) {

	if (rpc->rpc_direction == RPCDirection::rpc_in_invoke) {
	}
	else if (rpc->rpc_direction == RPCDirection::rpc_out_result) {
	}
}

void Leader::dispatch_append_heart_beat(RPC* rpc)
{
	if (rpc->rpc_direction == RPCDirection::rpc_in_invoke) {
	}
	else if (rpc->rpc_direction == RPCDirection::rpc_out_result) {
	}
}


void Leader::dispatch(RPC* rpc)
{
	std::lock_guard<std::mutex> locker(mu_leader_);

	if (!have_to_die_) {

		if (rpc->rpc_type == RPCTypeEnum::rpc_append_entry)
		{
			dispatch_append_entry(rpc);
		}
		// If I receive a request vote(// Another server is faster than I am. )
		else if (rpc->rpc_type == RPCTypeEnum::rpc_append_request_vote)
		{
			dispatch_request_vote(rpc);
		}
		// Another server establishes itself as a leader. 
		else if (rpc->rpc_type == RPCTypeEnum::rpc_append_heart_beat) {
			dispatch_append_heart_beat(rpc);
		}
		else
			Log::trace("Leader::dispatch - Wrong!!! type " + std::to_string(static_cast<int>(rpc->rpc_type)) + "\r\n");
	}
}