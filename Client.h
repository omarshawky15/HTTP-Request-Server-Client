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
    SOCKET serverSocket;
    IO *io;
    int init();
    int createRequest(HTTPBuilder*httpBuilder,std::string &request);
    int receiveResponse(HTTPBuilder *oldBuilder);
public :
    Client();
    int handleHTTPRequest(HTTPBuilder *httpBuilder);
    //int handleClientCmd(std::string &cmd);
    int createSocket(HTTPBuilder *builder);
    void shutdown() ;
    void setFilepathForClients(HTTPBuilder *httpBuilder);
};


#endif //ASSIGNMENT_1_CLIENT_H
