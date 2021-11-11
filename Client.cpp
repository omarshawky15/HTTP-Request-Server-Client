//
// Created by omara on 11/7/2021.
//

#include "Client.h"
//
// Created by omara on 11/7/2021.
//
// Initializes Winsock
int Client::init() {
    int iResult;
    WSADATA wsaData;

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
//parses cmds from input.txt and builds HTTP objects based on them
int Client::handleClientCmd(std::string &cmd) {
    std::string request;
    HTTPBuilder *newBuilder = Parser::parseClientCmd(cmd); //build an HTTP object that contains filepath-host-port number-GET/POST
    serverSocket = createSocket(newBuilder); //create socket for client based on host and port number in cmd
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Couldn't connect to host\n";
        return CONNECTION_CLOSED;
    }

    int result = Client::createRequest(newBuilder,request); //create a request based on the information in HTTP object
    if(result ==FILE_NOT_FOUND)return result;
    std::cout<<"\nrequest : \n" <<request; //print the request it's about to send
    result = ServerClientUtils::send(serverSocket,request.c_str(), request.size());
    if (result == SOCKET_ERROR) {
        std::cout << "Connection closed with the server" << std::endl;
        return CONNECTION_CLOSED;
    }
    result = Client::receiveResponse(newBuilder->getMethodType());//receive response (200/404) from server
    ServerClientUtils::shutdown(serverSocket); // shutdown client socket
    return result ;
}
//receive response (200/404) from server and parse it
int Client::receiveResponse(const std::string &methodType) {
    std::string response ;
    HTTPBuilder *newBuilder = new HTTPBuilder();
    std::string delim = END_OF_LINE;
    std::string RRLine;
    //read first line of response that tells if the request was successful or not
    int result = ServerClientUtils::recvWithDelim(serverSocket, delim,RRLine,0);
    if(result!=0)return result;
    response+=RRLine;
    //parse first line that tells if the request was successful or not
    Parser::parseResponseLine(RRLine, newBuilder);
    //if the request was Get then start reading header nad file content
    if (newBuilder->getResponseCode() == DEFAULT_HTTP_OK && methodType == GET_REQUEST) {
        delim += END_OF_LINE;
        std::string header;
        //receive header
        result = ServerClientUtils::recvWithDelim(serverSocket, delim,header,0);
        if(result!=0)return result;
        response+=header;
        //parsing header contents and storing it in HTTP object
        std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
        Parser::parseHeaderContents(headerLines, newBuilder);

        //receive file contents based on what I read from header (content length/type)
        ServerClientUtils::recvData(serverSocket, newBuilder, io,response,0);
    }
    //print all I've received
    std::cout << "\nresponse :" << std::endl << response;
    return result;
}
//creates request and store it in request string based on HTTP object contents
int Client::createRequest(HTTPBuilder *httpBuilder,std::string &request) {
    std::string body;
    // if request is POST then try to read the body (file) to check wether it exists or not and get its content length a
    if (httpBuilder->getMethodType() == POST_REQUEST) {
        int result ;
        result = httpBuilder->buildBody(io,body);
        if (result == FILE_NOT_FOUND) {
            std::cout << "Couldn't find filepath" << std::endl;
            return result ;
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

    return STATUS_OK;
}
//creates client socket based on host and port number stored in HTTP object
int Client::createSocket(HTTPBuilder *builder) {
    int iResult;
    struct addrinfo *result = nullptr, hints{};

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = WSAAPI::getaddrinfo(builder->getHostName().empty()? DEFAULT_HOST : builder->getHostName().c_str(),
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



