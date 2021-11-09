#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace  std ;
int main() {
    Server *server = new Server();
    server->listen();
    while(true);

    return 0;
}
