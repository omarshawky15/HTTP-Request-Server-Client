#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include "Client.h"
#include "Parser.h"

using namespace std;

void openConnectionThread(std::string cmd) {
    Client *client = new Client();
    HTTPBuilder *newBuilder = Parser::parseClientCmd(cmd);
    if(client->createSocket(newBuilder) !=0)return;
    client->handleHTTPRequest(newBuilder);
}
void testTimeoutConnections(std::string cmd){
    Client *client = new Client();
    HTTPBuilder *newBuilder = Parser::parseClientCmd(cmd);
    if(client->createSocket(newBuilder) !=0)return;
    while(true);
    client->handleHTTPRequest(newBuilder);
}
void multipleClientsOneRequest(std::string inputFilepath) {
    std::ifstream ifstream(inputFilepath);
    std::string cmd;
    std::thread threads[50];
    int idx = 0;
    if(ifstream.is_open() ==false){
        cout << "File not found" << endl ;
        return ;
    }
    while (std::getline(ifstream, cmd)) {
        cout <<cmd << endl ;
        //threads[idx++] = std::thread(testTimeoutConnections, cmd); // test timeout
        threads[idx++] = std::thread(openConnectionThread, cmd); //test multiple thread connections
        //openConnectionThread(cmd);
    }
   for (int i = 0; i < idx; i++)
        threads[i].join();
}

void oneClientsMultipleRequests(std::string &hostName, std::string &portNumber,std::string inputFilepath) {
    Client *client = new Client();
    HTTPBuilder *newBuilder = new HTTPBuilder();
    newBuilder->setHostName(hostName);
    newBuilder->setPortNumber(portNumber);
    if(client->createSocket(newBuilder) !=0)return;
    std::ifstream ifstream(inputFilepath);
    if(ifstream.is_open() ==false){
        cout << "File not found" << endl ;
        return ;
    }
    std::string cmd;
    while (std::getline(ifstream, cmd)) {
        cout <<cmd << endl ;
        newBuilder = Parser::parseClientCmd(cmd);
        newBuilder->setHostName(hostName);
        newBuilder->setPortNumber(portNumber);
        client->handleHTTPRequest(newBuilder);
    }
    client->shutdown(); // shutdown client socket
}

int main(int argc, char *argv[]) {
    std::string inputFilepath = "input_1.txt";
    std::string hostname = DEFAULT_HOST;
    std::string portNumber = DEFAULT_PORT;
    if (argc == 3) {
        hostname = argv[1];
        portNumber = argv[2];
        inputFilepath = "input_3.txt";
        oneClientsMultipleRequests(hostname,portNumber,inputFilepath);
    } else if (argc == 2) {
        inputFilepath = argv[1];
        multipleClientsOneRequest(inputFilepath);
    }else {
        inputFilepath = "input_1.txt";
        //multipleClientsOneRequest(inputFilepath);
        oneClientsMultipleRequests(hostname,portNumber,inputFilepath);

    }
    return 0;
}
