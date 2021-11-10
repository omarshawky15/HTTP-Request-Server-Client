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
    return 0;
}

Client::Client() {
    init();
    io = new IO();
}

int Client::handleClientCmd(std::string &cmd) {
    std::string request;
    HTTPBuilder *newBuilder = Parser::parseClientCmd(cmd);
    serverSocket = createSocket(newBuilder);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Couldn't connect to host" << std::endl;
        return CONNECTION_CLOSED;
    }

    int result = createRequest(newBuilder,request);
    if(result ==FILE_NOT_FOUND)return result;
    std::cout<<"\nrequest : \n" <<request;
    result = ServerClientUtils::send(serverSocket,request.c_str(), request.size());
    if (result == SOCKET_ERROR) {
        std::cout << "Connection closed with the server" << std::endl;
        return CONNECTION_CLOSED;
    }
    result = Client::receiveResponse(newBuilder->getMethodType());
    Client::shutdown();
    return result ;
}

int Client::receiveResponse(std::string &methodType) {
    std::string response ;
    HTTPBuilder *newBuilder = new HTTPBuilder();
    std::string delim = END_OF_LINE;
    std::string RRLine;
    int result = ServerClientUtils::recvWithDelim(serverSocket, delim,RRLine);
    if(result!=0)return result;
    response+=RRLine;
    Parser::parseResponseLine(RRLine, newBuilder);
    if (newBuilder->getResponseCode() == DEFAULT_HTTP_OK && methodType == GET_REQUEST) {
        delim += END_OF_LINE;
        std::string header;
        result = ServerClientUtils::recvWithDelim(serverSocket, delim,header);
        if(result!=0)return result;
        response+=header;
        std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
        Parser::parseHeaderContents(headerLines, newBuilder);
        ServerClientUtils::recvData(serverSocket, newBuilder, io,response);
    }
    std::cout << "\nresponse :" << std::endl << response;
    return result;
}

std::string Client::createRequest(HTTPBuilder *httpBuilder,std::string &request) {
    std::string body;
    if (httpBuilder->getMethodType() == POST_REQUEST) {
        int result = httpBuilder->buildBody(io,body);
        if (result == FILE_NOT_FOUND) {
            std::cout << "Couldn't find filepath" << std::endl;
        } else {
            httpBuilder->setContentLength(body.size());
            httpBuilder->setContentType(IO::GetMimeType(httpBuilder->getFilePath()));
        }
    }

    request += httpBuilder->buildRequestLine();
    request += httpBuilder->buildHeader(httpBuilder->getMethodType() == POST_REQUEST);

    if (httpBuilder->getMethodType() == POST_REQUEST) {
        request+=body;
    }
/*
    request += END_OF_LINE;
    request += END_OF_LINE;*/
    return request;
}

int Client::createSocket(HTTPBuilder *builder) {
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
        serverSocket = socket(ptr->ai_family, ptr->ai_socktype,
                              ptr->ai_protocol);
        if (serverSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            continue;
        }
        // Connect to server.
        iResult = WSAAPI::connect(serverSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    return serverSocket;
}
void Client::shutdown() {
    int result = WSAAPI::shutdown(serverSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
    }
}




