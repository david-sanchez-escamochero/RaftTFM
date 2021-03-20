#include "Candidate.h"

Candidate::Candidate(void* server)
{
	server_ = server;
	((Server*)server_)->increment_current_term();
}

Candidate::~Candidate()
{

}

void Candidate::start()
{

	send_request_vote_to_all_servers();
}


void Candidate::send_request_vote_to_all_servers() 
{
	// Send RPC's(Request vote) in parallel to each of the other servers in the cluster. 
	for (uint32_t count = 0; count < NUM_SERVERS; count++) {
		// If the receiver is not equal to sender...
		if (count != ((Server*)server_)->get_server_id()) {
			RPC rpc;
			rpc.rpc_type = RPCTypeEnum::rpc_append_request_vote;
			rpc.rpc_direction = RPCDirection::rpc_in_invoke;
			rpc.request_vote.argument_term_ = ((Server*)server_)->get_current_term();				// Candidate's term
			rpc.request_vote.argument_candidate_id_ = ((Server*)server_)->get_server_id();			// Candidate requesting vote
			rpc.request_vote.argument_last_log_index_ = ((Server*)server_)->get_last_applied();	// Index of candidate's last log entry (§5.4)
			rpc.request_vote.argument_last_log_term_ = 0;											// Term of candidate's last log entry (§5.4)

			send(&rpc,
				PORT_BASE + RECEIVER_PORT + count,
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
				REQUEST_FOR_VOTE,
				std::string(SERVER) + "(F)." + std::to_string(count)
			);
		}
	}
}

void Candidate::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
	((Server*)server_)->send(rpc, port, sender, action, receiver);
}

void Candidate::receive(RPC* rpc)
{
	
}


