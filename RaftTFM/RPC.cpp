#include "RPC.h"
#include "RPC.h"

RPC::RPC() 
{

}

std::string RPC::parse_from_enum_to_text(RPCTypeEnum type)
{
	std::string ret;
	switch (type) {
	case RPCTypeEnum::rpc_append_entry:
		ret = std::string(APPEND_ENTRY_TEXT);
		break;
	case RPCTypeEnum::rpc_append_request_vote:
		ret = std::string(REQUEST_VOTE_TEXT);
		break;
	case RPCTypeEnum::rpc_append_heart_beat:
		ret = std::string(HEART_BEAT_TEXT);
		break;
	case RPCTypeEnum::rpc_client_request_value:
		ret = std::string(CLIENT_REQUEST_VALUE_TEXT);
		break;
	case RPCTypeEnum::rpc_client_request_leader:
		ret = std::string(CLIENT_REQUEST_LEADER_TEXT);
		break;
	default:
		ret = std::string(UNKNOWN_TEXT);
	}
	return ret;
}
