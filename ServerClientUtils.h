//
// Created by omara on 11/10/2021.
//

#ifndef ASSIGNMENT_1_SERVERCLIENTUTILS_H
#define ASSIGNMENT_1_SERVERCLIENTUTILS_H
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "IO.h"
#include "HTTPBuilder.h"

class ServerClientUtils {
public :
#define CONNECTION_CLOSED 1
#define RECEIVE_FAILED 2
    static int recvWithDelim(SOCKET socket, std::string &delim,std::string &header);

    static bool checkRecvEnd(std::string &header, std::string &delim);

    static int send(SOCKET socket, const char *data, int size);

    static bool recvData(SOCKET socket, HTTPBuilder *builder, IO *io,std::string &data);
};


#endif //ASSIGNMENT_1_SERVERCLIENTUTILS_H
