//
// Created by omara on 11/7/2021.
//

#ifndef ASSIGNMENT_1_SERVER_H
#define ASSIGNMENT_1_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>
#include "parser.h"
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512
#define MAX_SERVER_CON 50

class Server {
private :
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE listenerThread ;
    DWORD dwThreadIdArray[50];
    Parser *parser ;
    IO *io;
    int init();
    DWORD threadListen(Server server);
    void threadHandleSocket(SOCKET clientSocket);
    std::string recvHeader(SOCKET clientSocket);
    bool checkHeaderEnd(std::string &header);
    bool recvData(SOCKET clientSocket,HTTPBuilder *builder);
    void sendResponse(bool success,SOCKET clientSocket,HTTPBuilder*httpBuilder);
    std::string buildBody(SOCKET socket, HTTPBuilder *pBuilder);
    std::string buildHeader(HTTPBuilder *pBuilder);
    int send(SOCKET socket,const char* data,int sendbuflen);
public :
    HANDLE requestThread [50];
    Server();
    SOCKET getSocket();
    int listen();

};


#endif //ASSIGNMENT_1_SERVER_H
