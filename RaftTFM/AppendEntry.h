#pragma once

#include <stdint.h>
#include "RaftDefs.h"

class AppendEntry
{
protected:
	//Arguments :
	uint32_t argument_term;							// Leader's term
	uint32_t argument_leader_id;					// So follower can redirect clients
	uint32_t argument_prev_log_index;				// Index of log entry immediately preceding	new ones
	uint32_t argument_prev_log_term;				// Term of argument_prev_log_index entry
	uint32_t argument_entries[MAX_LOG_ENTRIES];		// Log entries to store(empty for heartbeat; may send more than one for efficiency)
	uint32_t argument_leader_commit;				// Leader’s commitIndex
	// Results :
	uint32_t result_term;							// CurrentTerm, for leader to update itself
	uint32_t result_success;						// True if follower contained entry matching argument_prev_log_index and argument_prev_log_term
};

