#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include "Client.h"
#include "Parser.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void openConnection(std::string cmd) {
    Client *client = new Client();
    int result = client->handleClientCmd(cmd);
}

int main() {
    std::ifstream ifstream("input_1.txt");
    std::string cmd;
    std::thread threads[3];
    int idx =0 ;
    while (std::getline(ifstream, cmd)) {
        threads[idx++] = std::thread(openConnection,cmd);
    }
    for(int i=0 ;i<3;i++)threads[i].join();
    return 0;
}

#pragma clang diagnostic pop
