#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#include "Parser.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace  std ;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main() {
    Client *client = new Client();
    Parser *parser = new Parser ();
    while(true){
        string operation ;
        std::getline(std::cin,operation) ;
        cout <<"input = "<< operation << endl ;
        HTTPBuilder *builder = parser->parseClientCmd(operation);
        /*cout << builder->buildRequest() ;
        char buf [5] ={};
        while(builder->readFile(buf,5) !=nullptr){
            cout << buf ;
        }
        cout << endl ;*/
        int result = client->sendRequest(builder);
        if(result !=0){
            cout << "Connection terminated by the server" << endl ;
            return -1 ;
        }
    }

    return 0;
}
#pragma clang diagnostic pop
