#include "Follower.h"
#include <thread>
#include "Log.h"



Follower::Follower(void* server)
{
	server_					= server;
	have_to_die_			= false;
	receiving_heartbeats_	= ELECTION_TIME_OUT; // Tries alllowed without receiving heartbeats 
	i_have_already_voted	= false;
}

Follower::~Follower()
{
	have_to_die_ = true;
}


void Follower::start()
{
	std::thread thread_check_candidate(&Follower::check_candidate, this);
	thread_check_candidate.detach();	
}


void Follower::check_candidate() 
{
	while ( (receiving_heartbeats_) && (!have_to_die_) ){
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		receiving_heartbeats_--;
		Log::trace("(Follower." + std::to_string( ( (Server*) server_)->get_server_id() ) + ") checking for a candidate " + std::to_string(receiving_heartbeats_) + "\r\n");
	}		
	Log::trace("(Follower." + std::to_string( ( ( Server* )server_ )->get_server_id() ) +") Time out without receiving heartbeat\r\n");
	if (!have_to_die_) {
		// Inform server that state has changed. 
		((Server*)server_)->set_new_state(StateEnum::candidate_state);
	}	
}

void Follower::dispatch_append_request_vote(RPC* rpc)
{
	// If my term is out of date...
	if (rpc->request_vote.argument_term_ > ((Server*)server_)->get_current_term()) {
		((Server*)server_)->set_current_term(rpc->request_vote.argument_term_);
		i_have_already_voted = false;
	}

	rpc->rpc_direction = RPCDirection::rpc_out_result;
	// If term is out of date, or I have already voted. 
	if(  (rpc->request_vote.argument_term_ < ((Server*)server_)->get_current_term()) || (i_have_already_voted) )  {		
		rpc->request_vote.result_term_ = false;		
	}	
	else {
		rpc->request_vote.result_term_ = true;
		i_have_already_voted = true;
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
					std::string(REQUEST_FOR_VOTE) + std::string("(") + std::string(RESULT) + std::string(")"),
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

