#pragma once

#include <stdint.h>

class RequestVote
{
protected:
	// Arguments:
	uint32_t argument_term;				// Candidate's term
	uint32_t argument_candidate_id;		// Candidate requesting vote
	uint32_t argument_last_log_index;	// Index of candidate's last log entry (§5.4)
	uint32_t argument_last_log_term;	// Term of candidate's last log entry (§5.4)
	//Results :
	uint32_t result_term;				// CurrentTerm, for candidate to update itself
	uint32_t result_vote_ggranted;		// True means candidate received vote
};

