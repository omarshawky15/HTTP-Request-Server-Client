//
// Created by omara on 11/10/2021.
//

#include "ServerClientUtils.h"
/**
   *  @param  socket socket to receive header from.
   *  @param  delim  delimeter to stop receiving when received in header.
   *  @param  header carrier of receivied header byte from recv.
   *  @param  timeout  timeout value in milliseconds to ensure that server doesn't stay blocked on receiving from client a byte forever.
   *
   *  @return error code to indicate whether the receiving was successful or not
   *
   *  function to receive bytes until the delim is received
   *  before any bytes received a select function is implied with timeout to ensure the client doesn't sleep on sending
  */
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
            return RECEIVE_FAILED;
        }
    }
    return STATUS_OK;
}
/**
   *  @param  header part of header that was received until now received.
   *  @param  delim  delimeter to stop receiving when found in header.
   *
   *  @return boolean to indicate if delimiter was found in headers end
   *
   *  check if delim was received since last byte
  */
bool ServerClientUtils::checkRecvEnd(std::string &header, std::string &delim) {
    if (header.size() < delim.size())return false;
    else {
        return header.substr(header.size() - delim.size(), delim.size()) == delim;
    }
}
/**
   *  @param  socket socket to check timeout on.
   *  @param  timeout  timeout  timeout value in milliseconds to ensure that server doesn't stay blocked on receiving from client a byte forever.
   *
   *  @return boolean to indicate if timeout amount of time has passed or not at the end of calling select
   *
   *  function that forces timing out with select timeout
  */
bool ServerClientUtils::checkTimeout(SOCKET socket,int timeout){
    timeval connection_timer;
    connection_timer.tv_sec = timeout/MICRO_SEC;
    connection_timer.tv_usec = timeout%MICRO_SEC;
    //std ::cout << "seconds " << connection_timer.tv_sec <<"\nmicroseconds " << connection_timer.tv_usec<<"\n" ;
    fd_set fd_reader;
    FD_ZERO(&fd_reader);

    FD_SET(socket, &fd_reader);
    return WSAAPI::select(0, &fd_reader, nullptr, nullptr, &connection_timer)==0;
}
/**
   *  @param  socket socket to send data through.
   *  @param  data  data buffer to be sent
   *  @param  data  size of data buffer to be sent
   *  @return error code to define whether data sending was successful or not
   *
   *  sends data to client
  */
int ServerClientUtils::send(SOCKET socket, const char *data, int data_size) {
    int iResult = WSAAPI::send(socket, data, data_size, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        return iResult;
    }
    return iResult;
}
/**
   *  @param  socket socket to receive data on.
   *  @param  builder to know how much data (content-length) should be received from socket and where to store it (filepath)
   *  @param  data  string to concatenate total received data to be printed later as a whole
   *  @param  timeout timeout value in milliseconds to ensure that server doesn't stay blocked on receiving from client a byte forever.
   *
   *  @return error code to define whether data receiving was successful or not
   *
   *  receive file contents from client until content length reaches 0
  */
int ServerClientUtils::recvData(SOCKET socket, HTTPBuilder *builder, IO *io, std::string &data,int timeout) {
    int contentLength = builder->getContentLength(), iResult = 0, recvbuflen = std::min(DEFAULT_BUFLEN, contentLength);
    io->open(builder->getFilePath(), false);
    while (contentLength > 0) {
        char recvbuf[recvbuflen + 1];
        recvbuf[recvbuflen] = '\0';
        if(timeout>0&&checkTimeout(socket,timeout))return CONNECTION_TIMEOUT ;
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
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
            return RECEIVE_FAILED;
        }
    }
    io->close(false);
    return STATUS_OK;
}
/**
   *  @param  socket socket to be shutdown.
   *  @param  how int to indicate which way to shutdown on socket (SD_SEND,SD_RECEIVE,SD_BOTH)
   *
   *  shutdown socket passed as parameter
  */
// shutdown socket passed as parameter
void ServerClientUtils::shutdown(SOCKET socket,int how) {
    int result = WSAAPI::shutdown(socket, how);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
    }
}

