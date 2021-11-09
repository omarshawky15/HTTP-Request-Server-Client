//
// Created by omara on 11/7/2021.
//

#ifndef ASSIGNMENT_1_CLIENT_H
#define ASSIGNMENT_1_CLIENT_H


#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "HTTPBuilder.h"
class Client {
#define DEFAULT_PORT "80"
private :
    SOCKET connectSocket = INVALID_SOCKET;
    int init();
public :
    Client();
    int sendRequest(HTTPBuilder *builder);
    void get(HTTPBuilder *builder);
    int send (const std:: string& data) ;
    void post(HTTPBuilder *pBuilder);
    void shutdown() ;
};


#endif //ASSIGNMENT_1_CLIENT_H
