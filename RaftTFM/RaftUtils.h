#pragma once

#include "RaftDefs.h"
#include <stdio.h>
#include <string>


#define FOLLOWER_TEXT							"FOLLOWER"
#define	LEADER_TEXT								"LEADER"
#define CANDIDATE_TEXT							"CANDIDATE"
#define SERVER_TEXT								"SERVER"


std::string parse_state_to_string(StateEnum state) {
	std::string str; 
	if (state == StateEnum::candidate_state)
		str = std::string(CANDIDATE_TEXT);
	else if (state == StateEnum::follower_state)
		str = std::string(FOLLOWER_TEXT);
	else if (state == StateEnum::leader_state)
		str = std::string(LEADER_TEXT);
	else 
		str = std::string(UNKNOWN_TEXT);

	return str;
}
