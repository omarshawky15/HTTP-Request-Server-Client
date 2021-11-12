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
#include <mutex>
#include <utility>
#include<queue>
#include"Parser.h"
#include "ServerClientUtils.h"

#define DEFAULT_PORT "80"
#define MAX_SERVER_CONC 50
#define MAX_TIMEOUT 5000
class Server {
private :
    SOCKET listenSocket = INVALID_SOCKET;
    IO *io[MAX_SERVER_CONC];
    std::queue<int> avConc;
    std::mutex resMutex;
    int initSocket(std::string portNumber);
    void initDS();


    int sendResponse(bool success, SOCKET clientSocket, HTTPBuilder *httpBuilder,int threadId);

   // int send(SOCKET socket, const char *data, int sendbuflen);

    int receiveRequest(SOCKET clientSocket, HTTPBuilder *newBuilder,int threadId);

public :
    Server(std::string portNumber);


    void handleSocket(SOCKET clientSocket,int threadId);
    void listen();

    std::queue<int> &getAvConc();

    void setFilepathForServer(HTTPBuilder *httpBuilder);

    std::mutex *getResMutex();
};


#endif //ASSIGNMENT_1_SERVER_H
