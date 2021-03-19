#include "Follower.h"
#include <thread>
#include "Log.h"



Follower::Follower(void * server)
{
	server_			= server;
	have_to_die_	= false;
	

	std::thread thread_send_message(&Follower::send, this, this);
	thread_send_message.detach();
}

Follower::~Follower()
{
	have_to_die_ = true;
}

void Follower::send(void*)
{

}

void Follower::receive(RPC* rpc)
{
	dispatch(rpc);
}

void Follower::dispatch(RPC* rpc) 
{
		
}
