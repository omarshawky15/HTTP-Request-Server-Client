//
// Created by omara on 11/7/2021.
//

#ifndef ASSIGNMENT_1_CLIENT_H
#define ASSIGNMENT_1_CLIENT_H


#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include "HTTPBuilder.h"
#include "Parser.h"
#include "ServerClientUtils.h"
class Client {
#define DEFAULT_PORT "80"
private :
    SOCKET serverSocket = INVALID_SOCKET;
    IO *io;
    int init();
    int createRequest(HTTPBuilder*httpBuilder,std::string &request);
    int receiveResponse(const std::string &methodType);
    int createSocket(HTTPBuilder *builder);
public :
    Client();

    int handleClientCmd(std::string &cmd);

};


#endif //ASSIGNMENT_1_CLIENT_H
