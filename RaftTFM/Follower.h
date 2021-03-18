#pragma once


#include "IConnector.h"

class Follower : public IConnector
{
public:
	void send(void*);
	void* receive();

};

