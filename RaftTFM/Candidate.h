#pragma once

#include "IConnector.h"
#include "Server.h"

class Candidate : public IConnector
{
public:
	Candidate(void *server);
	~Candidate();
	void send(void*);
	void receive(RPC* rpc);
protected:
	void* server_;
};

