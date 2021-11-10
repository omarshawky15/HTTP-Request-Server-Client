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
#include "ServerClientUtils.h"
#define DEFAULT_PORT "80"
#define MAX_SERVER_CON 50

class Server {
private :
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE listenerThread ;
    DWORD dwThreadIdArray[50];
    IO *io;
    int init();
    DWORD threadListen(Server server);
    void threadHandleSocket(SOCKET clientSocket);
    /*std::string recvHeader(SOCKET clientSocket);
    bool checkHeaderEnd(std::string &header);
    bool recvData(SOCKET clientSocket,HTTPBuilder *builder);*/
    void sendResponse(bool success,SOCKET clientSocket,HTTPBuilder*httpBuilder);
    //std::string buildBody(HTTPBuilder *pBuilder);
    //std::string buildHeader(HTTPBuilder *pBuilder);
    int send(SOCKET socket,const char* data,int sendbuflen);
    int receiveRequest(SOCKET clientSocket,HTTPBuilder* newBuilder);
public :
    HANDLE requestThread [50];
    Server();
    SOCKET getSocket();
    int listen();

};


#endif //ASSIGNMENT_1_SERVER_H
