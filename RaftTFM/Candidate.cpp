#include "Candidate.h"
#include "Log.h"

Candidate::Candidate(void* server)
{	
	server_ = server;	
	Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") I am a CANDIDATE\r\n");
	there_is_leader_		= false;
	have_to_die_			= false;
	receive_votes_			= 1; // Starts in 1, because we dont send messages to myself. 	
	((Server*)server_)->increment_current_term();
}

Candidate::~Candidate()
{
	have_to_die_ = true;
	cv_send_request_vote_to_all_servers_.notify_all();


	Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") Destroying...\r\n");
	if (thread_send_request_vote_to_all_servers_.joinable())
		thread_send_request_vote_to_all_servers_.join();
	Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") Destroyed...\r\n");
}

void Candidate::start()
{
	thread_send_request_vote_to_all_servers_ = std::thread(&Candidate::send_request_vote_to_all_servers, this);	
}


void Candidate::reset_receive_votes() 
{
	receive_votes_ = 0; 
}

void Candidate::send_request_vote_to_all_servers() 
{
	while  ( (!there_is_leader_) && (!have_to_die_) ) 
	{						
		// Send RPC's(Request vote) in parallel to each of the other servers in the cluster. 
		for (uint32_t count = 0; count < NUM_SERVERS; count++) 
		{
			{
				std::lock_guard<std::mutex> locker_new_state(mu_candidate_);
				

				if ((!there_is_leader_) && (!have_to_die_)) {

					// If the receiver is not equal to sender...
					if (count != ((Server*)server_)->get_server_id()) {
						RPC rpc;
						rpc.server_id_origin = ((Server*)server_)->get_server_id();
						rpc.server_id_target = count;
						rpc.rpc_type = RPCTypeEnum::rpc_append_request_vote;
						rpc.rpc_direction = RPCDirection::rpc_in_invoke;
						rpc.request_vote.argument_term_ = ((Server*)server_)->get_current_term();				// Candidate's term
						rpc.request_vote.argument_candidate_id_ = ((Server*)server_)->get_server_id();			// Candidate requesting vote
						rpc.request_vote.argument_last_log_index_ = ((Server*)server_)->get_last_applied();		// Index of candidate's last log entry (§5.4)
						rpc.request_vote.argument_last_log_term_ = 0;											// Term of candidate's last log entry (§5.4)

						send(&rpc,
							PORT_BASE + RECEIVER_PORT + count,
							std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
							std::string(REQUEST_VOTE) + std::string("(") + std::string(INVOKE) + std::string(")"),
							std::string(SERVER) + "(F)." + std::to_string(count)
						);
					}
				}
			}
		}

		{			
			std::mutex mtx;
			std::unique_lock<std::mutex> lck(mtx);
			cv_send_request_vote_to_all_servers_.wait_for(lck, std::chrono::milliseconds(TIME_OUT_TERM));
		}

		
		{
			//std::lock_guard<std::mutex> locker_new_state(mu_candidate_);
			if ((!there_is_leader_) && (!have_to_die_)) {
				// Increments term. 
				((Server*)server_)->increment_current_term();
				// wait ramdomly. 
				/* initialize random seed: */
				srand((unsigned int)time(NULL));
				// 150-300(ms)
				uint32_t ramdom_timeout = rand() % MINIMUM_VALUE_RAMDOM_TIME_OUT + MINIMUM_VALUE_RAMDOM_TIME_OUT;
				std::this_thread::sleep_for(std::chrono::milliseconds(ramdom_timeout));
			}
		}
	}
	Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") send_request_vote_to_all_servers FINISHED.\r\n");
}

void Candidate::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
	((Server*)server_)->send(rpc, port, sender, action, receiver);
}

void Candidate::dispatch_append_entry(RPC* rpc)
{
	// And its terms is equal or highest than mine... 
	if (rpc->append_entry.argument_term_ >= ((Server*)server_)->get_current_term()) {
		Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") [Accepted]received an append_entry[term:" + std::to_string(rpc->append_entry.argument_term_) + " >= current_term:" + std::to_string(((Server*)server_)->get_current_term()) + "]\r\n");
		// Inform server that state has changed to follower.  
		((Server*)server_)->set_new_state(StateEnum::follower_state);

		rpc->rpc_direction = RPCDirection::rpc_out_result;
		rpc->append_entry.result_success_ = true;
		rpc->server_id_origin = ((Server*)server_)->get_server_id();
		rpc->server_id_target = rpc->append_entry.argument_leader_id_;

		send(rpc,
			PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
			std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
			std::string(APPEND_ENTRY) + std::string("(") + std::string(RESULT) + std::string(")"),
			std::string(SERVER) + "(C)." + std::to_string(rpc->append_entry.argument_leader_id_)
		);
		have_to_die_ = true;
		there_is_leader_ = true;
	}
	// Reject...
	else {
		Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") [Rejected] received an append_entry[term:" + std::to_string(rpc->append_entry.argument_term_) + " < current_term:" + std::to_string(((Server*)server_)->get_current_term()) + "]\r\n");
		rpc->rpc_direction = RPCDirection::rpc_out_result;
		rpc->append_entry.result_success_ = false;

		send(rpc,
			PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
			std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
			std::string(APPEND_ENTRY) + std::string("(") + std::string(RESULT) + std::string(")"),
			std::string(SERVER) + "(C)." + std::to_string(rpc->append_entry.argument_leader_id_)
		);
	}
}

void Candidate::dispatch_request_vote(RPC* rpc) 
{
	if (rpc->rpc_direction == RPCDirection::rpc_in_invoke) {
		// And its terms is equal or highest than mine... 
		if (rpc->request_vote.argument_term_ >= ((Server*)server_)->get_current_term()) {
			Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") [Accepted]received an request_vote[term:" + std::to_string(rpc->request_vote.argument_term_) + " >= current_term:" + std::to_string(((Server*)server_)->get_current_term()) + "]\r\n");
			// Inform server that state has changed to follower.  
			((Server*)server_)->set_new_state(StateEnum::follower_state);

			rpc->rpc_direction = RPCDirection::rpc_out_result;
			rpc->request_vote.result_vote_granted_ = true;
			rpc->server_id_origin = ((Server*)server_)->get_server_id();
			rpc->server_id_target = rpc->append_entry.argument_leader_id_;

			send(rpc,
				PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
				std::string(REQUEST_VOTE) + std::string("(") + std::string(RESULT) + std::string(")"),
				std::string(SERVER) + "(C)." + std::to_string(rpc->append_entry.argument_leader_id_)
			);
		}
		// Reject...
		else {
			Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") [Rejected]received an request_vote[term:" + std::to_string(rpc->request_vote.argument_term_) + " < current_term:" + std::to_string(((Server*)server_)->get_current_term()) + "]\r\n");
			rpc->rpc_direction = RPCDirection::rpc_out_result;
			rpc->request_vote.result_vote_granted_ = false;
			rpc->server_id_origin = ((Server*)server_)->get_server_id();
			rpc->server_id_target = rpc->append_entry.argument_leader_id_;


			send(rpc,
				PORT_BASE + RECEIVER_PORT + rpc->append_entry.argument_leader_id_,
				std::string(SERVER) + "(C)." + std::to_string(((Server*)server_)->get_server_id()),
				std::string(REQUEST_VOTE) + std::string("(") + std::string(RESULT) + std::string(")"),
				std::string(SERVER) + "(C)." + std::to_string(rpc->append_entry.argument_leader_id_)
			);
		}

	}
	// rpc_out_result
	else if ((rpc->rpc_direction == RPCDirection::rpc_out_result)) {
		// If someone voted for me. 
		if ((bool)rpc->request_vote.result_term_ == true) {
			receive_votes_++;
			// If I wins election. 	
			if (receive_votes_ >= MAJORITY) {
				Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") I have received just mayority of request vote: " + std::to_string(receive_votes_) + "\r\n");
				((Server*)server_)->set_new_state(StateEnum::leader_state);
				there_is_leader_ = true;
			}
		}
		// If I was rejected. 
		else {
			Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") Rejected request voted\r\n");
		}
	}
}

void Candidate::dispatch_append_heart_beat(RPC* rpc) 
{
	Log::trace("(Candidate." + std::to_string(((Server*)server_)->get_server_id()) + ") received heart_beat\r\n");
	((Server*)server_)->set_new_state(StateEnum::follower_state);
	there_is_leader_ = true;
}

void Candidate::receive(RPC* rpc)
{
	dispatch(rpc);
}

void Candidate::dispatch(RPC* rpc) 
{
	std::lock_guard<std::mutex> locker_new_state(mu_candidate_);

	if ((!there_is_leader_) && (!have_to_die_)) {
		// If I receive an append_entry(// Another server establishes itself as a leader. )
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
			Log::trace("Candidate::dispatch - Wrong!!! type " + std::to_string(static_cast<int>(rpc->rpc_type)) + "\r\n");

	}
}
