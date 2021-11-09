//
// Created by omara on 11/8/2021.
//

#ifndef ASSIGNMENT_1_HTTPBUILDER_H
#define ASSIGNMENT_1_HTTPBUILDER_H

#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include<sstream>

#define HOST_NAME_FIELD "Host:"
#define CONTENT_LENGTH_FIELD "Content-Length:"
#define CONTENT_TYPE_FIELD "Content-Type:"
class HTTPBuilder {
private :
#define DEFAULT_PORT "80"
#define DEFAULT_HOST "localhost"
#define DEFAULT_HTTP_VERSION "HTTP/1.1"
#define DEFAULT_HTTP_OK "200 OK"
#define DEFAULT_HTTP_NOT_FOUND "404 Not Found"
#define END_OF_LINE "\r\n"
    std::ifstream myFileStream;
    std::string methodType ;
    std::string hostName ;
    std::string filePath;
    std::string portNumber;
    std::string fileData;
    std::string responseCode;
    std::string contentType;
    bool isRequestBool;
    int contentLength;
    bool checkFile();
public :
#define GET_REQUEST "GET"
#define POST_REQUEST "POST"
    HTTPBuilder();
    HTTPBuilder* setMethodType(std::string methodType);

    HTTPBuilder* setHostName(std::string hostName);

    HTTPBuilder* setFilePath(std::string filePath);

    HTTPBuilder* setPortNumber(std::string portNumber);

    const std::string &getResponseCode() const;

    void setResponseCode(std::string responseCode);

    bool isRequest() const;

    void setIsRequest(bool isRequest);

    int getContentLength() const;

    void setContentLength(int contentLength);

    const std::string &getMethodType() const;

    const std::string &getHostName() const;

    const std::string &getFilePath() const;

    const std::string &getPortNumber() const;

    const std::string &getContentType() const;

    void setContentType(const std::string &contentType);

    std::string buildRequest();

    char * readFile(char * buf ,int bufSize);

    std::string buildResponse(bool response);
};


#endif //ASSIGNMENT_1_HTTPBUILDER_H
