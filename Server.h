//
// Created by omara on 11/7/2021.
//

#ifndef ASSIGNMENT_1_SERVER_H
#define ASSIGNMENT_1_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <utility>
#include<queue>
#include"Parser.h"
#include "ServerClientUtils.h"

#define DEFAULT_PORT "80"
#define MAX_SERVER_CONC 50
#define MAX_TIMEOUT 50
class Server {
private :
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE listenerThread;
    DWORD dwThreadIdArray[50];
    IO *io[MAX_SERVER_CONC];
    std::queue<int> avConc;
    int initSocket();
    void initDS();


    /*std::string recvHeader(SOCKET clientSocket);
    bool checkHeaderEnd(std::string &header);
    bool recvData(SOCKET clientSocket,HTTPBuilder *builder);*/
    int sendResponse(bool success, SOCKET clientSocket, HTTPBuilder *httpBuilder,int threadId);

    //std::string buildBody(HTTPBuilder *pBuilder);
    //std::string buildHeader(HTTPBuilder *pBuilder);
    int send(SOCKET socket, const char *data, int sendbuflen);

    int receiveRequest(SOCKET clientSocket, HTTPBuilder *newBuilder,int threadId);

public :
    Server();


    void handleSocket(SOCKET clientSocket,int threadId);
    void listen();

    std::queue<int> &getAvConc();
};


#endif //ASSIGNMENT_1_SERVER_H
