#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#include "Parser.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    Client *client = new Client();
    Parser *parser = new Parser();
    HTTPBuilder *oldBuilder = nullptr;
    while (true) {
        string operation;
        std::getline(std::cin, operation);
        int result =client->handleClientCmd(operation);
    }

    return 0;
}

#pragma clang diagnostic pop
