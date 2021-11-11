//
// Created by omara on 11/10/2021.
//

#include "ServerClientUtils.h"
// function to receive bytes until the delim is received
// before any bytes received a select function is implied with timeout to ensure the client doesn't sleep on sending
int ServerClientUtils::recvWithDelim(SOCKET socket, std::string &delim, std::string &header,int timeout) {
    int recvbuflen = 1, iResult;
    char recvbuf[recvbuflen + 1];
    recvbuf[recvbuflen] = '\0';
    while (true) {
        if(timeout>0&&ServerClientUtils::checkTimeout(socket,timeout))return CONNECTION_TIMEOUT;
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            header += recvbuf;
            if (ServerClientUtils::checkRecvEnd(header, delim))break;
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
            return CONNECTION_CLOSED;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            return RECEIVE_FAILED;
        }
    }
    return STATUS_OK;
}
// check if delim was received since last byte
bool ServerClientUtils::checkRecvEnd(std::string &header, std::string &delim) {
    if (header.size() < delim.size())return false;
    else {
        //std :: cout << header.substr(header.size() - 4, 4)  <<std::endl;
        return header.substr(header.size() - delim.size(), delim.size()) == delim;
    }
}
// function that forces timing out with select timeout
bool ServerClientUtils::checkTimeout(SOCKET socket,int timeout){
    timeval connection_timer;
    connection_timer.tv_sec = timeout;
    connection_timer.tv_usec = 0;
    fd_set fd_reader;
    FD_ZERO(&fd_reader);

    FD_SET(socket, &fd_reader);
    return WSAAPI::select(0, &fd_reader, nullptr, nullptr, &connection_timer)==0;
}
// send data to client
int ServerClientUtils::send(SOCKET socket, const char *data, int size) {
    int iResult = WSAAPI::send(socket, data, size, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return iResult;
    }
    return iResult;
}
// receive file contents from client until content length reaches 0
int ServerClientUtils::recvData(SOCKET socket, HTTPBuilder *builder, IO *io, std::string &data,int timeout) {
    int contentLength = builder->getContentLength(), iResult = 0, recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
    io->open(builder->getFilePath(), false);
    while (contentLength > 0) {
        char recvbuf[recvbuflen + 1];
        recvbuf[recvbuflen] = '\0';
        if(timeout>0&&checkTimeout(socket,timeout))return CONNECTION_TIMEOUT ;
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            contentLength -= iResult;
            recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
            if (!io->writeFile(recvbuf, iResult))return false;
            data += recvbuf;
        } else if (iResult == 0) {
            std::cout << "Connection closing :"<<WSAGetLastError() <<" ...\n";
            return CONNECTION_CLOSED;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            return RECEIVE_FAILED;
        }
    }
    io->close(false);
    return STATUS_OK;
}
// shutdown socket passed as parameter
void ServerClientUtils::shutdown(SOCKET socket) {
    int result = WSAAPI::shutdown(socket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
    }
}

