#include "Server.h"
#include <string>
#include "Log.h"
#include <string>
#include "RaftUtils.h"



#define SEMAPHORE_SERVER_DISPATCH		1
#define SEMAPHORE_SERVER_NEW_STATE		2	

Server::Server(uint32_t server_id)
{

	// Persisten state on all servers. 
	uint32_t current_term_	= 0;							// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	uint32_t voted_for_		= 0;							// CandidateId that received vote in current term(or null if none)
	memset(log_, 0, sizeof(log_));							// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	commit_index_			= 0;							// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	last_applied_			= 0	;							// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)	
	current_state_			= StateEnum::unknown_state;		// Starts as unknown. 
	new_state_				= StateEnum::unknown_state;		// Starts as unknown. 
	
	server_id_ = server_id;
	have_to_die_ = false;
		
	Log::trace("Started server ID:" + std::to_string(server_id_) + "\r\n");
}

Server::~Server()
{
	have_to_die_ = true;
	semaphore_dispatch_.notify(SEMAPHORE_SERVER_DISPATCH);
	semaphore_dispatch_.notify(SEMAPHORE_SERVER_NEW_STATE);

	if (thread_server_dispatch_.joinable()) {
		thread_server_dispatch_.join();
	}

	if (thread_check_new_state_.joinable()) {
		thread_check_new_state_.join();
	}
}

void Server::send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
	communication_.sendMessage(rpc, port, sender, action, receiver);
}

void Server::start() 
{	
	// TEST.
	//std::this_thread::sleep_for(std::chrono::milliseconds(30000));

	// Start server thread. 
	thread_server_dispatch_ = std::thread(&Server::dispatch, this);	

	// Start server check new state
	thread_check_new_state_ = std::thread(&Server::check_new_state, this);
	


	// Start new Rol(Follower at the beginning)
	set_new_state(StateEnum::follower_state);							


	receive();
}


void Server::dispatch()
{
	while(!have_to_die_)
	{
		semaphore_dispatch_.wait(SEMAPHORE_SERVER_DISPATCH);		
		// Get rpc(FIFO)
		RPC rpc = queue_.front();
		// Delete rcp from queue. 
		queue_.pop();
		{
			std::lock_guard<std::mutex> locker(mu_server_);

			if (connector_ != nullptr) {	
				connector_->receive(&rpc);
			}
		}
	}
}

void* Server::receive()
{
	while (!have_to_die_) {
		RPC rpc;		
		int error = communication_.receiveMessage( &rpc, PORT_BASE + RECEIVER_PORT + server_id_, std::string( SERVER ) + std::string( "." ) + std::to_string( get_server_id() ) );

		if (error) {
			Log::trace("Follower::receive - FAILED!!!  - error" + std::to_string(error) + "\r\n");
		}
		else {
			queue_.push(rpc);
			semaphore_dispatch_.notify(SEMAPHORE_SERVER_DISPATCH);
		}
	}
	return nullptr;
}

IConnector* Server::get_current_shape_sever(StateEnum state)
{
	// Before creating a new role, we have to delete previous one. 
	if (connector_ != nullptr)
		delete(connector_);

	IConnector* connector; 
	if (state == StateEnum::follower_state) {
		connector = new Follower(this);		
		Log::trace("Created Follower." + std::to_string(get_server_id()) + "\r\n");
	}
	else if (state == StateEnum::candidate_state) {
		connector = new Candidate(this);
		Log::trace("Created Candidate." + std::to_string(get_server_id()) + "\r\n");
	}
	else if (state == StateEnum::leader_state) {
		connector = new Leader(this);	
		Log::trace("Created Leader." + std::to_string( get_server_id() ) +"\r\n");
	}
	else {
		std::string color_name{ "GREEN" };
		Log::trace("Server::get_current_shape_sever -  FAILED!!! Unknow state:" + std::to_string(static_cast<int>(state)) + "%d\r\n");
		connector = NULL;
	}

	return connector;
}

uint32_t Server::get_server_id()
{
	return server_id_;
}

void Server::check_new_state() 
{
	while (!have_to_die_) {
		
		semaphore_new_state_.wait(SEMAPHORE_SERVER_NEW_STATE);
		{
			{
				std::lock_guard<std::mutex> locker_new_state(mu_new_state_);
				std::lock_guard<std::mutex> locker(mu_server_);
				if (current_state_ != new_state_) {
					Log::trace("Server(" + std::to_string(server_id_) + ") State changes from " + parse_state_to_string(current_state_) + " to " + parse_state_to_string(new_state_) + "\r\n");
					current_state_ = new_state_;
					connector_ = get_current_shape_sever(current_state_);
					connector_->start();
				}
			}
		}
	}
}


void Server::set_new_state(StateEnum state)
{			
	{
		std::lock_guard<std::mutex> locker_new_state(mu_new_state_);
		Log::trace("Server(" + std::to_string(server_id_) + ") New state has been requested\r\n");
		new_state_ = state;
		semaphore_new_state_.notify(SEMAPHORE_SERVER_NEW_STATE);
	}
}

void Server::increment_current_term()
{	
	current_term_++;
	Log::trace("Server(" + std::to_string(server_id_) + ") Increment term from " + std::to_string(current_term_ - 1) + " to " + std::to_string(current_term_) + "\r\n");	
}

uint32_t Server::get_current_term()
{
	return current_term_;
}

void Server::set_current_term(uint32_t term)
{
	current_term_ = term;
}


uint32_t Server::get_commit_index()
{
	return commit_index_;
}

uint32_t Server::get_last_applied()
{
	return last_applied_;
}

