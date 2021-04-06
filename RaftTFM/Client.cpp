#include "Client.h"
#include <iostream>
#include <string>
#include <fstream>

#include "RPC.h"
#include "ClientDefs.h"


Client::Client()
{
    have_to_die_ = false;
    leader_      = NO_LEADER_FOUND;
}

Client::~Client()
{
    have_to_die_ = true;

    if (thread_server_receive_leader_.joinable())
        thread_server_receive_leader_.join();
}

bool Client::start(std::string file_name, uint32_t client_id)
{
    bool ret    = true;
	file_name_  = file_name;	
    client_id_  = client_id;
    
    find_a_leader();

    send_request(file_name, leader_);

    return ret;
}

void Client::find_a_leader() {
    bool ret = false;
    leader_  = NO_LEADER_FOUND;

    // Start server receive leader 
    thread_server_receive_leader_ = std::thread(&Client::receive, this);
    uint32_t num_server = 0;

    do {
        send_request_to_find_a_leader(num_server++);
        {
            // Time waiting servers replay saying who is the leader...
            Tracer::trace("Client - Leader does not respond, error: " + std::to_string(ret) + "\r\n");
            Tracer::trace("Client - Waiting " + std::to_string(TIME_WAITING_A_LEADER) + "(ms) to retry...\r\n");
            std::mutex mtx;
            std::unique_lock<std::mutex> lck(mtx);
            cv_found_a_leader_.wait_for(lck, std::chrono::milliseconds(TIME_WAITING_A_LEADER));
        }

        if (num_server == NUM_SERVERS)
            num_server = 0;

    } while (leader_ == NO_LEADER_FOUND);    
}

void* Client::receive()
{
    while (!have_to_die_) {
        RPC rpc;
        int error = communication_.receiveMessage(&rpc, BASE_PORT + RECEIVER_PORT + client_id_, LEADER_TEXT);

        if (error) {
            Tracer::trace("Follower::receive - FAILED!!!  - error" + std::to_string(error) + "\r\n");
        }
        else {                        
            if (
                (rpc.rpc_direction == RPCDirection::rpc_out_result) && 
                (rpc.rpc_type == RPCTypeEnum::rpc_client_request_leader) && 
                (rpc.client_request.client_result_ == (uint32_t)true)                
                ) {
                leader_ = rpc.client_request.client_leader_;
                cv_found_a_leader_.notify_one();
            } 
            else if (
                (rpc.rpc_direction == RPCDirection::rpc_out_result) &&
                (rpc.rpc_type == RPCTypeEnum::rpc_client_request_value) &&
                (rpc.client_request.client_result_ == (uint32_t)true)
                ) {                
                cv_committed_value_.notify_one();
            }
        }
    }
    return nullptr;
}

void Client::send_request_to_find_a_leader(uint32_t num_server) {
    
    RPC rpc;
    rpc.rpc_direction = RPCDirection::rpc_in_invoke;
    rpc.rpc_type = RPCTypeEnum::rpc_client_request_leader;
    rpc.client_request.client_id_ = client_id_;

    int ret = NO_LEADER_FOUND;
    
    
    ret = communication_.sendMessage(&rpc, BASE_PORT + RECEIVER_PORT + num_server, CLIENT_TEXT, CLIENT_REQUEST_LEADER_TEXT, LEADER_TEXT);
    if (ret) {
        Tracer::trace("Client - Failed to request leader from server " + std::to_string(num_server) + ", error " + std::to_string(ret) + "\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }    
}

bool Client::send_request(std::string file_name, uint32_t leader_id)
{
    bool ret = true;
    if (file_name_ != "") {
                        
        std::string value;

        std::ifstream infile(file_name_);

        while (std::getline(infile, value))
        {
            Tracer::trace("Value proposed:" + value + "\r\n");
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            RPC rpc; 
            rpc.rpc_direction = RPCDirection::rpc_in_invoke;
            rpc.rpc_type = RPCTypeEnum::rpc_client_request_value;
            rpc.client_request.client_id_ = client_id_;
            rpc.client_request.client_value_ = atoi(value.c_str());
            cv_status time_out; 

            do {
                int ret = 0;
                do
                {
                    int ret = communication_.sendMessage(&rpc, BASE_PORT + RECEIVER_PORT + leader_id, CLIENT_TEXT, CLIENT_REQUEST_LEADER_TEXT, LEADER_TEXT);
                    if (ret) {
                        Tracer::trace("Client - Leader does not respond, error: " + std::to_string(ret) + "\r\n");
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                } while (ret);
                {

                    // Time waiting servers replay saying who is the leader...
                    std::mutex mtx;
                    std::unique_lock<std::mutex> lck(mtx);
                    time_out = cv_committed_value_.wait_for(lck, std::chrono::milliseconds(TIME_WAITING_COMMIT_VALUE));
                }
                if (time_out == std::cv_status::timeout) {
                    find_a_leader();
                }
            } while (time_out != std::cv_status::timeout);
        }        
    }
    else {
        Tracer::trace("Client::start - FAILED!!! file_name  == '' \r\n");
        ret = false;
    }

    return ret;
}
