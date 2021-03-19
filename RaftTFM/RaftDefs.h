#pragma once


#define NUM_SERVERS							5
#define MAX_LOG_ENTRIES						50
#define PORT_BASE							6000
#define SENDER_PORT							100
#define RECEIVER_PORT						200
#define MAJORITY							( ( NUM_NODES % 2 ) + 1 )
#define TIME_OUT_TO_RECEIVE_HEART_BEAT		3000


#define FOLLOWER							"FOLLOWER"
#define	LEADER								"LEADER"
#define CANDIDATE							"CANDIDATE"
#define SERVER								"SERVER"