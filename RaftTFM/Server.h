#pragma once

#include <stdint.h>
#include "RaftDefs.h"
#include "Command.h"

class Server
{
protected:
	// Persisten state on all servers. 
	uint32_t current_term;			// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	uint32_t voted_for;				// CandidateId that received vote in current term(or null if none)
	Command log[MAX_LOG_ENTRIES];		// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	uint32_t commit_index;			// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	uint32_t last_applied;			// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)

};

