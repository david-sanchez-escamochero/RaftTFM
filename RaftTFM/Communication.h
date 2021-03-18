#pragma once

#include <string>

#include "Log.h"

#define MSG_SUCCESS                                 0
#define MSG_ERROR_INITIALIZATION_SOCKET             1
#define MSG_ERROR_UNKNOWN_SERVER                    2
#define MSG_ERROR_CREATE_SOCKET                     3
#define MSG_ERROR_FAILED_TO_CONNECT_SERVER          4
#define MSG_ERROR_TO_ASSOCIATE_PORT_AND_IP_SOCKET   5
#define MSG_ERROR_TO_ENABLE_INGOING_CONNECTIONS     6
#define MSG_ERROR_TO_ACCEPT_INGOING_CONNECTIONS     7



class Communication
{
public: 
	Communication();
	//int sendMessage(Proposal* proposal, unsigned short port, std::string sender, std::string action, std::string receiver);
	//int receiveMessage(Proposal *proposal, unsigned short port, std::string receiver);
	
private: 
	char SendBuff[512], RecvBuff[512];

};

