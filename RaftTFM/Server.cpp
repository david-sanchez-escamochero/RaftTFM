#include "Server.h"
#include <string>
#include "Log.h"
#include <string>

Server::Server(uint32_t server_id)
{

	// Persisten state on all servers. 
	uint32_t current_term_	= 0;			// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	uint32_t voted_for_		= 0;			// CandidateId that received vote in current term(or null if none)
	memset(log_, 0, sizeof(log_));		// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	commit_index_			= 0;			// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	last_applied_			= 0;			// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)	
	state_					= StateEnum::follower_state;

	connector_ = get_current_shape_sever(state_);

	server_id_ = server_id;

		
	Log::trace("Starting server ID:" + std::to_string(server_id_) + "\r\n");
}

void Server::send(void* rpc)
{
	connector_->send(rpc);
}

void* Server::receive()
{
	return connector_->receive();
}

IConnector* Server::get_current_shape_sever(StateEnum state)
{
	IConnector* connector; 
	if (state == StateEnum::follower_state) {
		connector = new Follower();
		Log::trace("Created Follower\r\n");
	}
	else if (state == StateEnum::candidate_state) {
		connector = new Candidate();
		Log::trace("Created Candidate\r\n");
	}
	else if (state == StateEnum::leader_state) {
		connector = new Leader();	
		Log::trace("Created Leader\r\n");
	}
	else {
		std::string color_name{ "GREEN" };
		Log::trace("Server::get_current_shape_sever -  FAILED!!! Unknow state:" + std::to_string(static_cast<int>(state)) + "%d\r\n");
		connector = NULL;
	}

	return connector;
}
