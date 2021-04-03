#pragma once


enum class StateEnum { follower_state, leader_state, candidate_state, unknown_state };


#define NUM_SERVERS												2
#define MAX_LOG_ENTRIES											50
#define PORT_BASE												6000
#define SENDER_PORT												100
#define RECEIVER_PORT											200
#define MAJORITY												2//( ( NUM_SERVERS % 2 ) + 1 ) - 1  // -1 Because I do not send message to myself.
#define ELECTION_TIME_OUT										3//(s) 	A follower receives no communication over a period of time. 	
#define	MINIMUM_VALUE_RAMDOM_TIME_OUT							1000//(ms)
#define	TIME_OUT_CHECK_IF_THERE_IS_CANDIDATE_OR_LEADER			15000//(ms)
#define	TIME_OUT_IF_THERE_IS_CANDIDATE_OR_LEADER				1000//(ms)
#define	TIME_OUT_TERM											10000//(ms)
#define TIME_OUT_HEART_BEAT										2000//(ms)
#define TIME_OUT_LEADER_TERM									60000//(ms)
#define TIME_OUT_WAIT											2000//(ms)


#define FOLLOWER												"FOLLOWER"
#define	LEADER													"LEADER"
#define CANDIDATE												"CANDIDATE"
#define SERVER													"SERVER"
#define UNKNOWN													"UNKNOWN"

#define REQUEST_VOTE											"REQUEST_VOTE"
#define APPEND_ENTRY											"APPEND_ENTRY"
#define HEART_BEAT												"HEART_BEAT"
#define INVOKE													"INVOKE"
#define RESULT													"RESULT"


