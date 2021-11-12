#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace  std ;
int main(int argc, char *argv[] ) {
    std::string portNumber = DEFAULT_PORT;
    if(argc ==2)portNumber = argv[1];
    //std::cout << portNumber.c_str() << std::endl;
    Server *server = new Server(portNumber);
    server->listen();
    return 0;
}
