#include "Communication.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>


Communication::Communication() {
  
}

//int Message::sendMessage(Proposal *proposal, unsigned short port, std::string sender, std::string action, std::string receiver)
//{
//    
//    WSADATA wsaData;
//    SOCKET conn_socket;
//    struct sockaddr_in server;
//    struct hostent* hp;
//    int resp;
//    std::string str_trace;
//
//    //Inicializamos la DLL de sockets
//    resp = WSAStartup(MAKEWORD(1, 0), &wsaData);
//    if (resp) {
//        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + ". " + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Error socket initialization)\r\n";
//        log_->trace(str_trace);
//        return MSG_ERROR_INITIALIZATION_SOCKET;
//    }
//
//    //Obtenemos la IP del servidor... en este caso
//    // localhost indica nuestra propia máquina...
//    hp = (struct hostent*)gethostbyname("localhost");
//
//    if (!hp) {      
//        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Server not found)\r\n";
//        log_->trace(str_trace);
//        WSACleanup(); return MSG_ERROR_UNKNOWN_SERVER;
//    }
//
//    // Creamos el socket...
//    conn_socket = socket(AF_INET, SOCK_STREAM, 0);
//    if (conn_socket == INVALID_SOCKET) {
//        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Wrong socket created)\r\n";
//        log_->trace(str_trace);
//        WSACleanup(); return MSG_ERROR_CREATE_SOCKET;
//    }
//
//    memset(&server, 0, sizeof(server));
//    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
//    server.sin_family = hp->h_addrtype;
//    server.sin_port = htons(port);
//
//    // Nos conectamos con el servidor...
//    if (connect(conn_socket, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR) {
//        str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Failed to connect server)\r\n";
//        log_->trace(str_trace);
//        closesocket(conn_socket);
//        WSACleanup(); return MSG_ERROR_FAILED_TO_CONNECT_SERVER;
//    }
//    
//    memcpy(SendBuff, reinterpret_cast<const char *>(proposal), sizeof(Proposal));    
//
//    str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - OK] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "]\r\n";
//    log_->trace(str_trace);
//    send(conn_socket, SendBuff, sizeof(Proposal), 0);    
//
//    // Cerramos el socket y liberamos la DLL de sockets
//    closesocket(conn_socket);
//    WSACleanup();
//    return EXIT_SUCCESS;
//}
//
//int Message::receiveMessage(Proposal *proposal, unsigned short port, std::string receiver)
//{
//    WSADATA wsaData;
//    SOCKET conn_socket, comm_socket;
//    //SOCKET comunicacion;
//    struct sockaddr_in server;
//    struct sockaddr_in client;
//    struct hostent* hp;
//    int resp, stsize;
//    std::string str_trace;
//
//    //Inicializamos la DLL de sockets
//    resp = WSAStartup(MAKEWORD(1, 0), &wsaData);
//    if (resp) {
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Error socket initialization)\r\n";
//        log_->trace(str_trace);
//        return MSG_ERROR_INITIALIZATION_SOCKET;
//    }
//
//    //Obtenemos la IP que usará nuestro servidor... 
//    // en este caso localhost indica nuestra propia máquina...
//    hp = (struct hostent*)gethostbyname("localhost");
//
//    if (!hp) {        
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Server not found)\r\n";
//        log_->trace(str_trace);
//        WSACleanup(); return MSG_ERROR_UNKNOWN_SERVER;
//    }
//
//    // Creamos el socket...
//    conn_socket = socket(AF_INET, SOCK_STREAM, 0);
//    if (conn_socket == INVALID_SOCKET) {
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Wrong socket created)\r\n";
//        log_->trace(str_trace);
//        WSACleanup(); return MSG_ERROR_CREATE_SOCKET;
//    }
//
//    memset(&server, 0, sizeof(server));
//    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
//    server.sin_family = hp->h_addrtype;
//    server.sin_port = htons(port);
//
//    // Asociamos ip y puerto al socket
//    resp = bind(conn_socket, (struct sockaddr*) & server, sizeof(server));
//    if (resp == SOCKET_ERROR) {
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Failed to connect server)\r\n";
//        log_->trace(str_trace);
//        closesocket(conn_socket); WSACleanup();
//        return MSG_ERROR_TO_ASSOCIATE_PORT_AND_IP_SOCKET;
//    }
//
//    if (listen(conn_socket, 1) == SOCKET_ERROR) {        
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Failed to connect server)\r\n";
//        log_->trace(str_trace);
//        closesocket(conn_socket); WSACleanup();
//        return MSG_ERROR_TO_ENABLE_INGOING_CONNECTIONS;
//    }
//
//    // Aceptamos conexiones entrantes    
//    stsize = sizeof(struct sockaddr);
//    comm_socket = accept(conn_socket, (struct sockaddr*) & client, &stsize);
//    if (comm_socket == INVALID_SOCKET) {
//        str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "](Failed to accept ingoing conection)\r\n";
//        log_->trace(str_trace);
//        closesocket(conn_socket); WSACleanup();
//        return MSG_ERROR_TO_ACCEPT_INGOING_CONNECTIONS;
//    }
//    
//    // Como no vamos a aceptar más conexiones cerramos el socket escucha
//    closesocket(conn_socket);
//    
//    
//    recv(comm_socket, RecvBuff, sizeof(RecvBuff), 0);
//    Proposal *proposal_aux = reinterpret_cast<Proposal*>(RecvBuff);            
//    proposal->set_id(proposal_aux->get_id());
//    proposal->set_nack(proposal_aux->get_nack());
//    proposal->set_none(proposal_aux->get_none());
//    proposal->set_proposal_number(proposal_aux->get_proposal_number());
//    proposal->set_value(proposal_aux->get_value());
//    proposal->set_current_proposal_number(proposal_aux->get_current_proposal_number());
//    
//    str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - OK] Proposal[" + std::to_string(proposal->get_proposal_number()) + "." + std::to_string(proposal->get_id()) + ", " + proposal->get_value() + "]\r\n";
//    log_->trace(str_trace);
//    // Cerramos el socket de la comunicacion
//    closesocket(comm_socket);
//
//    // Cerramos liberia winsock
//    WSACleanup();
//    return (EXIT_SUCCESS);
//}
