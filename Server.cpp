//
// Created by omara on 11/7/2021.
//

#include "Server.h"


Server::Server() {
    Server::initSocket();
    Server::initDS();

}

//initializes resources to be used by clients' threads
void Server::initDS() {
    for (int i = 0; i < MAX_SERVER_CONC; i++) {
        io[i] = new IO();
        avConc.push(i);
    }

}

//initializes listen socket
int Server::initSocket() {

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
    iResult = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
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

//function for threads to execute servers functions withing thread
// server ptr is passed and the client socket that the server should work on from withing the thread
// while thread id is responsible for telling the thread which object to use
void runClientThread(Server *hostServer, SOCKET clientSocket, int threadId) {
    std::string starting = "\nthread (" + Parser::intToStr(threadId) + ") starting...\n";
    std::string ending = "\nthread (" + Parser::intToStr(threadId) + ") ending...\n";
    std::cout << starting;
    hostServer->handleSocket(clientSocket, threadId);
    std::cout << ending;
    hostServer->getAvConc().push(threadId);
}

// start listening on listen socket
void Server::listen() {
    if (WSAAPI::listen(listenSocket, 50) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    } else
        while (WSAAPI::listen(listenSocket, MAX_SERVER_CONC) != SOCKET_ERROR) {
            std::cout << "listening\n";
            SOCKET clientSocket = INVALID_SOCKET;
            // wait and accept a client socket
            clientSocket = accept(listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                printf("accept failed: %d\n", WSAGetLastError());
                closesocket(listenSocket);
                WSACleanup();
                return;
            } else {
                // if a new client came and there's no free resources in the queue
                // then shutdown the client socket and continue listening for new clients
                // until a resource is available
                if (avConc.size() == 0) {
                    std::cout << "\nServer is busy now...Rejecting Connection\n";
                    shutdown(clientSocket);
                    continue;
                }
                // if resources available then pop a resource from queue and assign it to the new thread
                std::cout << "\nConnection established\n";
                int threadId = avConc.front();
                avConc.pop();
                std::thread(runClientThread, this, clientSocket, threadId).detach();
            }
        }
}

// main function to handle clients
void Server::handleSocket(SOCKET clientSocket, int threadId) {
    //TODO timeout timer here
    while (true) {

        std::string response;
        int result;
        HTTPBuilder *requestBuilder = new HTTPBuilder();
        result = receiveRequest(clientSocket, requestBuilder, threadId); //receive client's request
        if (result == CONNECTION_CLOSED) {
            break;
        } else if (result == CONNECTION_TIMEOUT) {
            std::cout << "\nConnection Timed-out...\nClosing Connection\n";
            break;
        } else if (sendResponse(result == 0, clientSocket, requestBuilder, threadId) == SOCKET_ERROR) { // send response
            std::cout << "\nProblem with socket when sending response:" << WSAGetLastError()
                      << "...\nClosing Connection\n";
            break;
        }
    }
    // if server reached here means that connection timed out so shutdown the clients socket
    ServerClientUtils::shutdown(clientSocket);
}
//receive request from client
int Server::receiveRequest(SOCKET clientSocket, HTTPBuilder *newBuilder, int threadId) {
    std::string request;
    std::string delim = END_OF_LINE;
    std::string RRLine;
    int timeout = MAX_TIMEOUT / (MAX_SERVER_CONC - Server::getAvConc().size()); // calculate timeout
    //receive first line of request to decide if it's GET or POST
    int result = ServerClientUtils::recvWithDelim(clientSocket, delim, RRLine, timeout);
    if (result != 0)return result;
    request += RRLine;
    Parser::parseResponseLine(RRLine, newBuilder);
    delim += END_OF_LINE;
    std::string header;
    //receive header (based on the assumption of : if GET/POST there's always a header)
    result = ServerClientUtils::recvWithDelim(clientSocket, delim, header, timeout);
    if (result != 0)return result;
    request += header;
    std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
    // parse each line of the header and store the data extracted in HTTP object
    Parser::parseHeaderContents(headerLines, newBuilder);
    //receive file content if the request is POST
    if (newBuilder->getMethodType() == POST_REQUEST) {
        result = ServerClientUtils::recvData(clientSocket, newBuilder, io[threadId], request, timeout);//TODO change 5
    }
    //print request received
    std::cout << "\nrequest :\n" << request;
    return result;
}
// send response to the client
int Server::sendResponse(bool success, SOCKET clientSocket, HTTPBuilder *httpBuilder, int threadId) {
    std::string body;
    // if it's a GET request, check if the file exists and if it does then load it and get its content length
    if (success && httpBuilder->getMethodType() == GET_REQUEST) {
        std::string filepath = httpBuilder->getFilePath();
        int result = httpBuilder->buildBody(io[threadId], body);
        if (result == FILE_NOT_FOUND)success = false;
        else {
            httpBuilder->setContentLength(body.size());
            httpBuilder->setContentType(IO::GetMimeType(filepath));
        }
    }
    //start building first line of response based on whether GET/POST receiving succeded
    // and in case of GET based on whether the file exists or not
    std::string response = httpBuilder->buildResponse(success);
    if (success && httpBuilder->getMethodType() == GET_REQUEST) {
        std::string header = httpBuilder->buildHeader(true);
        response += header;
        response += body;
    } else {
        response += END_OF_LINE;
    }

    std::cout << "\nresponse :\n";
    std::cout << response;
    // send response to the client
    return Server::send(clientSocket, response.c_str(), response.size());
}

// sending response function
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
// gets queue of thread resources
std::queue<int> &Server::getAvConc() {
    return avConc;
}
