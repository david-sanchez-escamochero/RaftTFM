#pragma once

enum class RPCTypeEnum { rpc_append_entry, rpc_append_request_vote};
enum class RPCDirection { rpc_in_invoke, rpc_out_result};

#include "AppendEntry.h"
#include "RequestVote.h"

class RPC
{
public:
	RPC();

	RPCTypeEnum rpc_type; 
	RPCDirection rpc_direction;

	union
	{
		AppendEntry append_entry;
		RequestVote request_vote;
	};
};
