//
// Created by omara on 11/7/2021.
//

#include "Client.h"
//
// Created by omara on 11/7/2021.
//

int Client::init() {
    int iResult;
    WSADATA wsaData;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

}

Client::Client() {
    init();
}
int Client::sendRequest(HTTPBuilder *builder) {
    int iResult;
    struct addrinfo *result = nullptr, hints{};

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = WSAAPI::getaddrinfo(builder->getHostName().c_str(),
                                  builder->getPortNumber().empty() ? DEFAULT_PORT : builder->getPortNumber().c_str(),
                                  &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        // Connect to server.
        iResult = WSAAPI::connect(connectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    if (builder->getMethodType() == GET_REQUEST)get(builder);
    else post(builder);
    return 0;
}


void Client::get(HTTPBuilder *builder) {
    int result = Client::send(builder->buildRequest());
    if (result) {
        return ;
    }
    Client::shutdown();
}

void Client::post(HTTPBuilder *builder) {
    int result = Client::send(builder->buildRequest());
    if (result) {
        return ;
    }
    char buf[5] = {};
    while (builder->readFile(buf, 5) != nullptr) {
        result = Client::send(buf);
        if (result) {
            return ;
        }
    }
    shutdown();
}

int Client::send(const std::string& data)  {
    int iResult = WSAAPI::send(connectSocket, data.c_str(), data.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1 ;
    }
}

void Client::shutdown()  {
    int result = WSAAPI::shutdown(connectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
    }
}



