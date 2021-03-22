#pragma once

#include <stdint.h>
#include "RaftDefs.h"
#include "Command.h"
#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
#include "IConnector.h"
#include "Semaphore.h"
#include <queue>

enum class StateEnum { follower_state, leader_state, candidate_state};


class Server
{

public: 
	Server(uint32_t server_id);
	~Server();
	void		send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void*		receive();
	void		start();
	IConnector* get_current_shape_sever(StateEnum state);
	uint32_t    get_server_id();
	void		set_new_state(StateEnum state);
	std::mutex	mu_;
	void		increment_current_term();
	uint32_t	get_current_term();
	void		set_current_term(uint32_t term);
	uint32_t	get_commit_index();
	uint32_t	get_last_applied();

protected:
	// Persisten state on all servers. 
	uint32_t current_term_;			// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	uint32_t voted_for_;				// CandidateId that received vote in current term(or null if none)
	Command log_[MAX_LOG_ENTRIES];		// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	uint32_t commit_index_;			// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	uint32_t last_applied_;			// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)
	StateEnum current_state_; 
	//StateEnum new_state_;


private: 
	
	IConnector*		connector_;
	uint32_t		server_id_;
	Communication	communication_;
	bool			have_to_die_;	
	void			dispatch();
	Semaphore		semaphore_;
	queue<RPC>		queue_;
	
};

