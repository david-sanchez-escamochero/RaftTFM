#pragma once

#include "RaftDefs.h"
#include <stdio.h>
#include <string>


#define FOLLOWER							"FOLLOWER"
#define	LEADER								"LEADER"
#define CANDIDATE							"CANDIDATE"
#define SERVER								"SERVER"


std::string parse_state_to_string(StateEnum state) {
	std::string str; 
	if (state == StateEnum::candidate_state)
		str = std::string(CANDIDATE);
	else if (state == StateEnum::follower_state)
		str = std::string(FOLLOWER);
	else if (state == StateEnum::leader_state)
		str = std::string(LEADER);
	else 
		str = std::string(UNKNOWN);

	return str;
}
