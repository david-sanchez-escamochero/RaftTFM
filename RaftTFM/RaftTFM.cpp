// RaftTFM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Server.h"
#include <stdint.h>
#include <string>
#include "Communication.h"
#include "RPC.h"



int main(int argc, char** argv)
{
    std::cout << "RAFT Test...\n";
    if (argc < 2) {
        printf("Usage:\r\n");
        printf("Param (1): id server\r\n");        
        printf("Example RaftTest.exe 1\r\n");
    }
    else {
        Server server(std::stoi(argv[1]));
        server.start();


        // Test(sender)
        //Communication communication;
        //RPC rpc;
        //rpc.rpc_type = RPCTypeEnum::rpc_append_request_vote;
        //rpc.rpc_direction = RPCDirection::rpc_out_result;
        //communication.sendMessage(&rpc, 6000, "", "", "");


        // Test(receiver)
        //Communication communication;
        //RPC rpc;
        //communication.receiveMessage(&rpc, 6000, "");
    }    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
