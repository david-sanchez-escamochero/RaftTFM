#pragma once

#include <stdint.h>
#include <string>

class ClientRequest
{
public:
	// Arguments:
	uint32_t client_id;
	std::string client_value;
	// Results: 
	uint32_t client_result;
	uint32_t client_leader;
};



