//
// Created by omara on 11/7/2021.
//

#include "Server.h"


Server::Server() {
    init();
    parser = new Parser();
}

int Server::init() {

    int iResult;
    WSADATA wsaData;
    struct addrinfo *result = nullptr, hints{};


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerrorA(iResult));
        WSACleanup();
        return 2;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (listenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 3;
    }

    // Setup the TCP listening socket
    iResult = bind(listenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 4;
    }
    freeaddrinfo(result);
    return 0;
}

SOCKET Server::getSocket() {
    return listenSocket;
}

DWORD WINAPI threadDecodeHTTPRequest(/*LPVOID lpParameter*/ SOCKET clientSocket) {
    std::cout << "decoding...\n";
    //SOCKET clientSocket = *((SOCKET *) lpParameter);
    int iResult;
    std::string receivedRequest;
    while (true) {
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            receivedRequest += recvbuf;
            std::cout << "received : \n" << recvbuf;
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
            break;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            break;
        }
    }
    std::cout << "received : \n" << receivedRequest << std::endl;
}

DWORD Server::threadListen(Server server) {
    //Server server = *((Server *) lpParameter);
    SOCKET listenSocket = server.getSocket();
    SOCKET clientSocket = INVALID_SOCKET;
    while (WSAAPI::listen(listenSocket, MAX_SERVER_CON) != SOCKET_ERROR) {
        std::cout << "listening\n";

        // Accept a client socket
        clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        } else {
            std::cout << "connection established\n";
            /*server.requestThread[0] = CreateThread(
                    nullptr,                   // default security attributes
                    0,                      // use default stack size
                    threadDecodeHTTPRequest,       // thread function name
                    &clientSocket,          // argument to thread function
                    0,                      // use default creation flags
                    nullptr);*/
            //threadDecodeHTTPRequest(clientSocket);
            Server::threadHandleSocket(clientSocket);
        }
    }
    return 0;
}

int Server::listen() {
    if (WSAAPI::listen(listenSocket, 50) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    } else {
        /*listenerThread = CreateThread(
                nullptr,                   // default security attributes
                0,                      // use default stack size
                threadListenFoo,       // thread function name
                this,          // argument to thread function
                0,                      // use default creation flags
                &dwThreadIdArray[0]);*/
        threadListen(*this);
    }
    return 0;
}

void Server::threadHandleSocket(SOCKET clientSocket) {
    //TODO timeout timer here
    std::string header = Server::recvHeader(clientSocket);
    std::cout << "header :" << std::endl << header << std::endl;

    HTTPBuilder httpBuilder = Parser::parseHeader(header);
}

std::string Server::recvHeader(SOCKET clientSocket) {
    std::string header;
    int recvbuflen = 1, iResult;
    char recvbuf[recvbuflen + 1];
    recvbuf[recvbuflen] = '\0';
    while (true) {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            header += recvbuf;
            if (checkHeaderEnd(header))break;
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
            break;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            break;
        }
    }
    return header;
}

bool Server::checkHeaderEnd(std::string &header) {
    if (header.size() < 4)return false;
    else {
        //std :: cout << header.substr(header.size() - 4, 4)  <<std::endl;
        return header.substr(header.size() - 4, 4) == "\r\n\r\n";
    }
}

