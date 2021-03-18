#pragma once

#include <stdint.h>
#include "RaftDefs.h"
#include "Command.h"
#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
#include "IConnector.h"

enum class StateEnum { follower_state, leader_state, candidate_state};


class Server
{

public: 
	Server();
	void send(void *);
	void* receive();

protected:
	// Persisten state on all servers. 
	uint32_t current_term_;			// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	uint32_t voted_for_;				// CandidateId that received vote in current term(or null if none)
	Command log_[MAX_LOG_ENTRIES];		// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	uint32_t commit_index_;			// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	uint32_t last_applied_;			// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)
	StateEnum state_; 


private: 

	IConnector* get_current_shape_sever(StateEnum state);
	IConnector* connector_;

	//Candidate*	candidate_;
	//Follower*	follower_;
	//Leader*		leader_;

};

