#pragma once

#include "IConnector.h"

class Candidate : public IConnector
{
public:
	void send(void*);
	void* receive();
};

