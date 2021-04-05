#include "Communication.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "Tracer.h"


Communication::Communication() {

    COMPILE_TIME_ASSERT(sizeof(RPC) > sizeof(MAX_SIZE_BUFFER));
}

int Communication::sendMessage(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver)
{
    
    WSADATA wsaData;
    SOCKET conn_socket;
    struct sockaddr_in server;
    struct hostent* hp;
    int resp;
    std::string str_trace;

    //Inicializamos la DLL de sockets
    resp = WSAStartup(MAKEWORD(1, 0), &wsaData);
    if (resp) {
        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + ")) - FAILED] - (Error socket initialization)\r\n";
        Tracer::trace(str_trace);
        return MSG_ERROR_INITIALIZATION_SOCKET;
    }

    //Obtenemos la IP del servidor... en este caso
    // localhost indica nuestra propia máquina...
    hp = (struct hostent*)gethostbyname("localhost");

    if (!hp) {      
        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))  - FAILED] - (Server not found)\r\n";
        Tracer::trace(str_trace);
        WSACleanup(); return MSG_ERROR_UNKNOWN_SERVER;
    }

    // Creamos el socket...
    conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_socket == INVALID_SOCKET) {
        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))   - FAILED] - (Wrong socket created)\r\n";
        Tracer::trace(str_trace);
        WSACleanup(); return MSG_ERROR_CREATE_SOCKET;
    }

    memset(&server, 0, sizeof(server));
    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(port);

    // Nos conectamos con el servidor...
    if (connect(conn_socket, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR) {
        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] - (Failed to connect server)\r\n";
        Tracer::trace(str_trace);
        closesocket(conn_socket);
        WSACleanup(); return MSG_ERROR_FAILED_TO_CONNECT_SERVER;
    }
    
    
    memcpy(SendBuff, reinterpret_cast<const char *>(rpc), sizeof(RPC));    

    
    str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - OK]\r\n";
    Tracer::trace(str_trace);
    send(conn_socket, SendBuff, sizeof(RPC), 0);    

    // Cerramos el socket y liberamos la DLL de sockets
    closesocket(conn_socket);
    WSACleanup();
    return EXIT_SUCCESS;
}

int Communication::receiveMessage(RPC* rpc, unsigned short port, std::string receiver)
{
    WSADATA wsaData;
    SOCKET conn_socket, comm_socket;
    //SOCKET comunicacion;
    struct sockaddr_in server;
    struct sockaddr_in client;
    struct hostent* hp;
    int resp, stsize;
    std::string str_trace;

    //Inicializamos la DLL de sockets
    resp = WSAStartup(MAKEWORD(1, 0), &wsaData);
    if (resp) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] - (Error socket initialization)\r\n";
        Tracer::trace(str_trace);
        return MSG_ERROR_INITIALIZATION_SOCKET;
    }

    //Obtenemos la IP que usará nuestro servidor... 
    // en este caso localhost indica nuestra propia máquina...
    hp = (struct hostent*)gethostbyname("localhost");

    if (!hp) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Server not found)\r\n";
        Tracer::trace(str_trace);
        WSACleanup(); return MSG_ERROR_UNKNOWN_SERVER;
    }

    // Creamos el socket...
    conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_socket == INVALID_SOCKET) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED](Wrong socket created)\r\n";
        Tracer::trace(str_trace);
        WSACleanup(); return MSG_ERROR_CREATE_SOCKET;
    }

    memset(&server, 0, sizeof(server));
    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(port);

    // Asociamos ip y puerto al socket
    resp = bind(conn_socket, (struct sockaddr*) & server, sizeof(server));
    if (resp == SOCKET_ERROR) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to connect server)\r\n";
        Tracer::trace(str_trace);
        closesocket(conn_socket); WSACleanup();
        return MSG_ERROR_TO_ASSOCIATE_PORT_AND_IP_SOCKET;
    }

    if (listen(conn_socket, 1) == SOCKET_ERROR) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to connect server)\r\n";
        Tracer::trace(str_trace);
        closesocket(conn_socket); WSACleanup();
        return MSG_ERROR_TO_ENABLE_INGOING_CONNECTIONS;
    }

    // Aceptamos conexiones entrantes    
    stsize = sizeof(struct sockaddr);
    comm_socket = accept(conn_socket, (struct sockaddr*) & client, &stsize);
    if (comm_socket == INVALID_SOCKET) {
        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to accept ingoing conection)\r\n";
        Tracer::trace(str_trace);
        closesocket(conn_socket); WSACleanup();
        return MSG_ERROR_TO_ACCEPT_INGOING_CONNECTIONS;
    }

    // Como no vamos a aceptar más conexiones cerramos el socket escucha
    closesocket(conn_socket);


    recv(comm_socket, RecvBuff, sizeof(RPC), 0);
    RPC* rpc_aux = reinterpret_cast<RPC*>(RecvBuff);
    *rpc = *rpc_aux;

    std::string str_aux;
    if (rpc->rpc_type == RPCTypeEnum::rpc_append_entry) {
        str_aux = "APPEND_ENTRY";           
    }
    else if (rpc->rpc_type == RPCTypeEnum::rpc_append_request_vote) {
        str_aux = "REQUEST_VOTE";
    }
    else if (rpc->rpc_type == RPCTypeEnum::rpc_append_heart_beat) {
        str_aux = "HEART_BEAT";
    }
    
    str_trace = "[>>>>> Received [" + str_aux + "] to (" + receiver + "(" + std::to_string(port) + ")) - from (" + std::string(SERVER_TEXT) + "." + std::to_string(rpc->server_id_origin) + ")   - OK] \r\n";
    Tracer::trace(str_trace);
    // Cerramos el socket de la comunicacion
    closesocket(comm_socket);

    // Cerramos liberia winsock
    WSACleanup();
    return (EXIT_SUCCESS);
}
