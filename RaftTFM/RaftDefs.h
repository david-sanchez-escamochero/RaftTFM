#pragma once


#define NUM_SERVERS							5
#define MAX_LOG_ENTRIES						50
#define PORT_BASE							6000
#define SENDER_PORT							100
#define RECEIVER_PORT						200
#define MAJORITY							( ( NUM_SERVERS % 2 ) + 1 ) - 1  // -1 Because I do not send message to myself.
#define ELECTION_TIME_OUT					3//(s) 	A follower receives no communication over a period of time. 	


#define FOLLOWER							"FOLLOWER"
#define	LEADER								"LEADER"
#define CANDIDATE							"CANDIDATE"
#define SERVER								"SERVER"

#define REQUEST_FOR_VOTE					"REQUEST_FOR_VOTE"
#define APPEND_ENTRY						"APPEND_ENTRY"
#define INVOKE								"INVOKE"
#define RESULT								"RESULT"