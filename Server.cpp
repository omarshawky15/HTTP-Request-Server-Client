//
// Created by omara on 11/7/2021.
//

#include "Server.h"


Server::Server() {
    init();
    io = new IO();
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
    while (true) {

        std::string response;
        int result;
        HTTPBuilder *requestBuilder = new HTTPBuilder();
        result = receiveRequest(clientSocket, requestBuilder);
        if (result == 1) {
            break;
        } else
            sendResponse(result == 0, clientSocket, requestBuilder);
    }
}

int Server::receiveRequest(SOCKET clientSocket, HTTPBuilder *newBuilder) {
    std::string request;
    std::string delim = END_OF_LINE;
    std::string RRLine;
    int result = ServerClientUtils::recvWithDelim(clientSocket, delim, RRLine);
    if (result !=0)return result;
    request += RRLine;
    Parser::parseResponseLine(RRLine, newBuilder);
    delim += END_OF_LINE;
    std::string header ;
    result = ServerClientUtils::recvWithDelim(clientSocket, delim,header);
    if (result !=0)return result;
    request += header;
    std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
    Parser::parseHeaderContents(headerLines, newBuilder);
    if (newBuilder->getMethodType() == POST_REQUEST) {
        result = ServerClientUtils::recvData(clientSocket, newBuilder, io, request);
    }
    std::cout << "\nrequest :\n" << request;
    return result;
}

/*
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
        return header.substr(header.size() - 4, 4) == "\r\n\r\n";
    }
}

bool Server::recvData(SOCKET clientSocket, HTTPBuilder *builder) {
    int contentLength = builder->getContentLength(), iResult = 0, recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
    std::string data;
    io->open(builder->getFilePath(), false);
    while (contentLength > 0) {
        char recvbuf[recvbuflen + 1];
        recvbuf[recvbuflen] = '\0';
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            contentLength -= iResult;
            recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
            if (!io->writeFile(recvbuf, iResult))return false;
            std::cout << recvbuf;
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
            return false;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }
    }
    io->close(false);
    char extraEOF[4];
    recv(clientSocket, extraEOF, 6, 0);
    return true;
}
*/
void Server::sendResponse(bool success, SOCKET clientSocket, HTTPBuilder *httpBuilder) {
    std::string body ;
    if (success && httpBuilder->getMethodType() == GET_REQUEST) {
        std::string filepath = httpBuilder->getFilePath();
        int result =httpBuilder->buildBody(io,body);
        if (result == FILE_NOT_FOUND)success= false ;
        else {
            httpBuilder->setContentLength(body.size());
            httpBuilder->setContentType(IO::GetMimeType(filepath));
        }
    }
    std::string response = httpBuilder->buildResponse(success);
    if (success && httpBuilder->getMethodType() == GET_REQUEST) {
        std::string header = httpBuilder->buildHeader(true);
        response += header;
        //int result= httpBuilder->buildBody(io,body);
        response+=body;
    } else {
        response += END_OF_LINE;
    }

    /* response += END_OF_LINE;
     response += END_OF_LINE;
     response += END_OF_LINE;*/
    std::cout << "\nresponse :\n";
    std::cout << response;
    Server::send(clientSocket, response.c_str(), response.size());
}

/*
std::string Server::buildBody(HTTPBuilder *httpBuilder) {
    std::string body;
    int contentLength = httpBuilder->getContentLength(), sendbuflen = std::min(contentLength, DEFAULT_BUFLEN);
    char buf[sendbuflen + 1];
    io->open(httpBuilder->getFilePath(), true);
    while (contentLength > 0 && (sendbuflen = io->readFile(buf, sendbuflen)) > 0) {
        body += buf;
        contentLength -= sendbuflen;
    }
    return body;
}
*/

int Server::send(SOCKET clientSocket, const char *data, int size) {
    int iResult = WSAAPI::send(clientSocket, data, size, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return iResult;
    }
    return iResult;
}

/*
std::string Server::buildHeader(HTTPBuilder *httpBuilder) {
    std::string header;
    header += CONTENT_TYPE_FIELD;
    header += " " + httpBuilder->getContentType();
    header += END_OF_LINE;

    header += CONTENT_LENGTH_FIELD;
    header += " " + Parser::intToStr(httpBuilder->getContentLength());
    header += END_OF_LINE;

    header+=HOST_NAME_FIELD;
    header += " " +httpBuilder->getHostName() +":" + httpBuilder->getPortNumber();
    header += END_OF_LINE;

    header += END_OF_LINE;

    return header;
}
*/
SOCKET Server::getSocket() {
    return listenSocket;
}
