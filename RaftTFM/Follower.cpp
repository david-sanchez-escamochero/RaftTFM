#include "Follower.h"
#include <thread>
#include "Log.h"




Follower::Follower(void* server)
{	
	server_											= server;
	Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") I am a Follower\r\n");
	have_to_die_									= false;
	receiving_heartbeats_							= ELECTION_TIME_OUT; // Tries alllowed without receiving heartbeats 
	last_time_stam_taken_miliseconds_				= duration_cast<milliseconds>(system_clock::now().time_since_epoch());	
	count_check_if_there_is_candidate_or_leader_	= 0;
}

Follower::~Follower()
{
	have_to_die_ = true;
	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	if (thread_check_candidate_.joinable()) {
		thread_check_candidate_.join();
	}
}


void Follower::start()
{
	thread_check_candidate_ = std::thread(&Follower::check_if_there_is_candidate_or_leader, this);	
}


void Follower::check_if_there_is_candidate_or_leader() 
{
	count_check_if_there_is_candidate_or_leader_ = 0;
	while (!have_to_die_) {
		{
			std::lock_guard<std::mutex> locker(mu_follower_);
			milliseconds current_time_stam_taken_miliseconds = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

			Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") Waiting if there is candidate or leader " + std::to_string(count_check_if_there_is_candidate_or_leader_++) + "...\r\n");

			if ((abs(last_time_stam_taken_miliseconds_.count() - current_time_stam_taken_miliseconds.count())) > TIME_OUT_CHECK_IF_THERE_IS_CANDIDATE_OR_LEADER)
			{
				Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") Time out without receiving messages from Candidate or Leader\r\n");
				((Server*)server_)->set_new_state(StateEnum::candidate_state);
				have_to_die_ = true;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_OUT_IF_THERE_IS_CANDIDATE_OR_LEADER));		
	}
}


void Follower::dispatch_append_request_vote(RPC* rpc)
{
	// Set as result. 
	rpc->rpc_direction = RPCDirection::rpc_out_result;

	// If term is out of date
	if (rpc->request_vote.argument_term_ < ((Server*)server_)->get_current_term()){ 
		rpc->request_vote.result_term_ = false;
		rpc->request_vote.result_term_ = ((Server*)server_)->get_current_term();
		Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") Term is out of date " + std::to_string(rpc->request_vote.argument_term_) + " < " + std::to_string(((Server*)server_)->get_current_term()) +"\r\n");
	}
	// I have already voted.
	else if ( ((Server*)server_)->get_voted_for() != NONE ){
		rpc->request_vote.result_term_ = false;
		rpc->request_vote.result_term_ = ((Server*)server_)->get_current_term();
		Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") I have already voted:" + std::to_string(((Server*)server_)->get_voted_for()) + "\r\n");
	}
	// Candidate's term is updated...
	// CandidateID is not null. 
	// Candidate's log is at least as up-to-date receivers's log, grant vote.
	else if ( 
		(rpc->request_vote.argument_term_ >= ((Server*)server_)->get_current_term()) && 
		(rpc->request_vote.argument_candidate_id_ != NONE)							 //&& 
		//(rpc->request_vote.argument_last_log_index_ == 0)							 && TODO: ?¿?¿?¿?¿?¿?¿?¿?¿?¿?
		//(rpc->request_vote.argument_last_log_term_ == 0)								TODO: ?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿
	   )
	{		
		((Server*)server_)->set_current_term(rpc->request_vote.argument_term_);
		((Server*)server_)->set_voted_for(rpc->request_vote.argument_candidate_id_);
		Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") Vote granted to :" + std::to_string(rpc->request_vote.argument_candidate_id_) + "\r\n");
	}

	
}

void Follower::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
	(((Server*)server_)->send(rpc, port, sender, action, receiver));
}

void Follower::receive(RPC* rpc)
{	
	dispatch(rpc);
}


void Follower::dispatch(RPC* rpc) 
{
	std::lock_guard<std::mutex> locker(mu_follower_);
	last_time_stam_taken_miliseconds_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	count_check_if_there_is_candidate_or_leader_ = 0;

	if (rpc->rpc_direction == RPCDirection::rpc_in_invoke) {
		switch (rpc->rpc_type)
		{
			case RPCTypeEnum::rpc_append_entry:
				break;
			case RPCTypeEnum::rpc_append_request_vote:
				dispatch_append_request_vote(rpc);
				send(rpc,
					PORT_BASE + RECEIVER_PORT + rpc->request_vote.argument_candidate_id_,
					std::string(SERVER) + "(F)." + std::to_string(((Server*)server_)->get_server_id()),					
					std::string(REQUEST_VOTE) + std::string("(") + std::string(RESULT) + std::string(")"),
					std::string(SERVER) + "(C)." + std::to_string(rpc->request_vote.argument_candidate_id_)
				);																				
				break;
			case RPCTypeEnum::rpc_append_heart_beat:
				break;
			default:
				break;
		}		
	}
	else {
		Log::trace("Follower::dispatch - Wrong!!! direction of message " + std::to_string(static_cast<int>(rpc->rpc_direction)) + "\r\n");
	}
}

