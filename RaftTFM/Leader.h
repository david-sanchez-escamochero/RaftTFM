#pragma once

#include "RaftDefs.h"
#include <stdint.h>
#include "IConnector.h"


class Leader : public IConnector
{

public: 
	void send(void*);
	void* receive();

protected: 
	//  Volatile state on leaders 
	// (Reinitialized after election)
	uint32_t next_index_[NUM_SERVERS];	// For each server, index of the next log entry	to send to that server(initialized to leader last log index + 1)
	uint32_t match_index_[NUM_SERVERS];	// For each server, index of highest log entry known to be replicated on server	(initialized to 0, increases monotonically)
};

