//
// Created by omara on 11/10/2021.
//

#include "ServerClientUtils.h"

int ServerClientUtils::recvWithDelim(SOCKET socket, std::string &delim,std::string &header) {
    int recvbuflen = 1, iResult;
    char recvbuf[recvbuflen + 1];
    recvbuf[recvbuflen] = '\0';
    while (true) {
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            header += recvbuf;
            if (ServerClientUtils::checkRecvEnd(header, delim))break;
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
            return CONNECTION_CLOSED ;
        } else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            return RECEIVE_FAILED ;
        }
    }
    return STATUS_OK;
}

bool ServerClientUtils::checkRecvEnd(std::string &header, std::string &delim) {
    if (header.size() < delim.size())return false;
    else {
        //std :: cout << header.substr(header.size() - 4, 4)  <<std::endl;
        return header.substr(header.size() - delim.size(), delim.size()) == delim;
    }
}

int ServerClientUtils::send(SOCKET socket,const char *data, int size) {
    int iResult = WSAAPI::send(socket, data, size, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return iResult;
    }
    return iResult;
}

bool ServerClientUtils::recvData(SOCKET socket, HTTPBuilder *builder,IO *io,std::string &data) {
    int contentLength = builder->getContentLength(), iResult = 0, recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
    io->open(builder->getFilePath(), false);
    while (contentLength > 0) {
        char recvbuf[recvbuflen + 1];
        recvbuf[recvbuflen] = '\0';
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            contentLength -= iResult;
            recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
            if (!io->writeFile(recvbuf, iResult))return false;
            data+=recvbuf;
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
    io->close(false);
    return STATUS_OK;
}

