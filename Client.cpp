//
// Created by omara on 11/7/2021.
//

#include "Client.h"

//
// Created by omara on 11/7/2021.
//
/**
   *  @return error code to indicate if initializing the library was successful or not
   *
   *  Initializes Winsock
  */
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
/**
   *  @param  httpBuilder HTTP object to carry necessary data for request.
   *  @return error code to indicate if sending the request was successful or not
   *
   *  sends request to the server
  */
int Client::handleHTTPRequest(HTTPBuilder *httpBuilder) {
    std::string request;
    int result = Client::createRequest(httpBuilder, request); //create a request based on the information in HTTP object
    if (result == FILE_NOT_FOUND)return result;
    //print the request it's about to send
    std::string printRequest = "\n\n" + IO::getTime() + "request :\n" + request;
    std::cout << printRequest;
    result = ServerClientUtils::send(serverSocket, request.c_str(), request.size());
    if (result == SOCKET_ERROR) {
        std::cout << "Connection closed with the server" << std::endl;
        return CONNECTION_CLOSED;
    }
    //receive response (200/404) from server
    result = Client::receiveResponse(httpBuilder);
    return result;
}
/**
   *  @param  oldBuilder HTTP object to carry necessary data from the request that was sent before.
   *  @return error code to indicate if receiving the requeest was successful or not
   *
   *  receive response (200/404) from server and parse it
  */
int Client::receiveResponse(HTTPBuilder *oldBuilder) {
    std::string response;
    HTTPBuilder *newBuilder = new HTTPBuilder();
    std::string delim = END_OF_LINE;
    std::string RRLine;
    if(oldBuilder->getMethodType() ==POST_REQUEST)delim+=END_OF_LINE;
    //read first line of response that tells if the request was successful or not
    int result = ServerClientUtils::recvWithDelim(serverSocket, delim, RRLine, 0);
    if (result != 0)return result;
    response += RRLine;
    //parse first line that tells if the request was successful or not
    Parser::parseResponseLine(RRLine, newBuilder);
    //if the request was Get then start reading header nad file content
    if (newBuilder->getResponseCode() == DEFAULT_HTTP_OK && oldBuilder->getMethodType() == GET_REQUEST) {
        delim += END_OF_LINE;
        std::string header;
        //receive header
        result = ServerClientUtils::recvWithDelim(serverSocket, delim, header, 0);
        if (result != 0)return result;
        response += header;
        //parsing header contents and storing it in HTTP object
        std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
        Parser::parseHeaderContents(headerLines, newBuilder);
        newBuilder->setFilePath(oldBuilder->getFilePath());
        Client::setFilepathForClients(newBuilder);
        //receive file contents based on what I read from header (content length/type)
        ServerClientUtils::recvData(serverSocket, newBuilder, io, response, 0);
    }
    //print all I've received
    std::string printResponse = "\n\n" + IO::getTime() + "response :\n" + response;
    std::cout << printResponse;
    //std::cout << "\nresponse :" << std::endl << response;
    return result;
}
/**
   *  @param  request  string of the request that will be sent.
   *  @param  httpBuilder HTTP object to carry necessary data for the request.
   *  @return error code to indicate if receiving the request was successful or not
   *
   *  creates request and store it in request string based on HTTP object contents
  */
int Client::createRequest(HTTPBuilder *httpBuilder, std::string &request) {
    std::string body;
    // if request is POST then try to read the body (file) to check wether it exists or not and get its content length a
    if (httpBuilder->getMethodType() == POST_REQUEST) {
        int result;
        result = httpBuilder->buildBody(io, body);
        if (result == FILE_NOT_FOUND) {
            std::cout << "Couldn't find filepath" << std::endl;
            return result;
        } else {
            httpBuilder->setContentLength(body.size());
            httpBuilder->setContentType(IO::getMimeType(httpBuilder->getFilePath()));
        }
    }

    request += httpBuilder->buildRequestLine();
    request += httpBuilder->buildHeader(httpBuilder->getMethodType() == POST_REQUEST);

    if (httpBuilder->getMethodType() == POST_REQUEST) {
        request += body;
    }

    return STATUS_OK;
}
/**
   *  @param  builder HTTP object to carry necessary data for creating the socket such as hostname and port number.
   *  @return error code to indicate if receiving the request was successful or not
   *
   *  creates client socket based on host and port number stored in HTTP object
  */
int Client::createSocket(HTTPBuilder *builder) {
    int iResult;
    struct addrinfo *result = nullptr, hints{};

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = WSAAPI::getaddrinfo(builder->getHostName().empty() ? DEFAULT_HOST : builder->getHostName().c_str(),
                                  builder->getPortNumber().empty() ? DEFAULT_PORT : builder->getPortNumber().c_str(),
                                  &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
      //  WSACleanup();
        return 1;
    }
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        serverSocket = socket(ptr->ai_family, ptr->ai_socktype,
                              ptr->ai_protocol);
        if (serverSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
         //   WSACleanup();
            continue;
        }
        // Connect to server.
        iResult = WSAAPI::connect(serverSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("socket couldn't connect to server with error: %ld\n", WSAGetLastError());
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    return 0;
}
/**
   *  @param  httpBuilder HTTP object that carries filepath to be changed.
   *
   *  adds an c_ prefix to files written by the client to help defining them during testing
  */
void Client::setFilepathForClients(HTTPBuilder*httpBuilder){
    std::string filepath = httpBuilder->getFilePath();
    int fileNameIdx = filepath.find_last_of("/");
    if (std::string::npos != fileNameIdx) {
        filepath = filepath.substr(0,fileNameIdx+1)+"c_"+filepath.substr(fileNameIdx+1);
        httpBuilder->setFilePath(filepath);
    }
}
/**
   *  shutdowns client socket
  */
void Client::shutdown(){
    ServerClientUtils::shutdown(serverSocket,SD_SEND); // shutdown client socket
}
