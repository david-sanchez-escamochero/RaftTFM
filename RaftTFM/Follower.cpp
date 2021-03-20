#include "Follower.h"
#include <thread>
#include "Log.h"



Follower::Follower(void* server)
{
	server_					= server;
	have_to_die_			= false;
	receiving_heartbeats_	= ELECTION_TIME_OUT; // Tries alllowed without receiving heartbeats 
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
		Log::trace("(Follower." + std::to_string(((Server*)server_)->get_server_id()) + ") checking for a candidate " + std::to_string(receiving_heartbeats_) + "\r\n");
	}		
	Log::trace("(Follower." + std::to_string( ( ( Server* )server_ )->get_server_id() ) +")Time out without receiving heartbeat\r\n");
	if (!have_to_die_) {
		// Inform server that state has changed. 
		((Server*)server_)->set_new_state(StateEnum::candidate_state);
	}	
}

void Follower::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{

}

void Follower::receive(RPC* rpc)
{	
	dispatch(rpc);
}


void Follower::dispatch(RPC* rpc) 
{
		
}
