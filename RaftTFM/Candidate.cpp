#include "Candidate.h"

Candidate::Candidate(void* server)
{
	server_ = server;
	((Server*)server_)->increment_current_term();
	there_is_not_leader_	 = true;
	have_to_die_			= false;
	receive_votes_			= 1; // Starts in 1, because we dont send messages to myself. 
}

Candidate::~Candidate()
{
	have_to_die_ = true;
}

void Candidate::start()
{
	send_request_vote_to_all_servers();
}


void Candidate::reset_receive_votes() 
{
	receive_votes_ = 0; 
}

void Candidate::send_request_vote_to_all_servers() 
{
	while  ( (there_is_not_leader_) && (!have_to_die_) ) {
		// Send RPC's(Request vote) in parallel to each of the other servers in the cluster. 
		for (uint32_t count = 0; count < NUM_SERVERS; ( count++ && (!have_to_die_) ) ) {
			// If the receiver is not equal to sender...
			if (count != ((Server*)server_)->get_server_id()) {
				RPC rpc;
				rpc.rpc_type = RPCTypeEnum::rpc_append_request_vote;
				rpc.rpc_direction = RPCDirection::rpc_in_invoke;
				rpc.request_vote.argument_term_ = ((Server*)server_)->get_current_term();				// Candidate's term
				rpc.request_vote.argument_candidate_id_ = ((Server*)server_)->get_server_id();			// Candidate requesting vote
				rpc.request_vote.argument_last_log_index_ = ((Server*)server_)->get_last_applied();		// Index of candidate's last log entry (§5.4)
				rpc.request_vote.argument_last_log_term_ = 0;											// Term of candidate's last log entry (§5.4)

				send(&rpc,
					PORT_BASE + RECEIVER_PORT + count,
					std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
					std::string(REQUEST_FOR_VOTE) + std::string("(") + std::string(INVOKE) + std::string(")"),
					std::string(SERVER) + "(F)." + std::to_string(count)
				);
			}
		}

		// Increments term. 
		((Server*)server_)->increment_current_term();
		// wait ramdomly. 
		/* initialize random seed: */
		srand((unsigned int)time(NULL));
		// 150-300(ms)
		uint32_t ramdom_timeout = rand() % 150 + 150;
		std::this_thread::sleep_for(std::chrono::milliseconds(ramdom_timeout));
	}
}

void Candidate::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
	((Server*)server_)->send(rpc, port, sender, action, receiver);
}

void Candidate::receive(RPC* rpc)
{
	// If I receive an append_entry(// Another server establishes itself as a leader. )
	if (rpc->rpc_type == RPCTypeEnum::rpc_append_entry)
	{
		// And its terms is equal or highest than mine... 
		if (rpc->append_entry.argument_term_ >= ((Server*)server_)->get_current_term()) {
			// Inform server that state has changed to follower.  
			((Server*)server_)->set_new_state(StateEnum::follower_state);

			rpc->rpc_direction = RPCDirection::rpc_out_result;
			rpc->append_entry.result_success_ = true;

			send(rpc,
				PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
				std::string(APPEND_ENTRY) + std::string("(") + std::string(RESULT) + std::string(")"),
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id())
			);
		}
		// Reject...
		else {

			rpc->rpc_direction = RPCDirection::rpc_out_result;
			rpc->append_entry.result_success_ = false;

			send(rpc,
				PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
				std::string(APPEND_ENTRY) + std::string("(") + std::string(RESULT) + std::string(")"),
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id())
			);
		}
	}

	// If I wins election. 	
	else if ((rpc->rpc_type == RPCTypeEnum::rpc_append_request_vote) && ((bool)rpc->request_vote.result_term_ == true)) {
		receive_votes_++;
		if (receive_votes_ >= MAJORITY) {
			((Server*)server_)->set_new_state(StateEnum::leader_state);
			there_is_not_leader_ = false;
		}
	}
	// Another server establishes itself as a leader. 
	else if (rpc->rpc_type == RPCTypeEnum::rpc_append_heart_beat) {
		((Server*)server_)->set_new_state(StateEnum::follower_state);
		there_is_not_leader_ = false;
	}
}


