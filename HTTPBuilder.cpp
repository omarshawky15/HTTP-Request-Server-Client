//
// Created by omara on 11/8/2021.
//

#include "HTTPBuilder.h"

HTTPBuilder::HTTPBuilder() {
    contentLength = 0;
    fileData = "";
    methodType = GET_REQUEST;
    hostName = DEFAULT_HOST;
    portNumber = DEFAULT_PORT;
}
std::string HTTPBuilder::buildHeader(bool containsBody) {
    std::string header;

    if(containsBody) {
        header += CONTENT_TYPE_FIELD;
        header += " " + getContentType();
        header += END_OF_LINE;

        header += CONTENT_LENGTH_FIELD;
        header += " " + HTTPBuilder::intToStr(getContentLength());
        header += END_OF_LINE;
    }
    header+=HOST_NAME_FIELD;
    header += " " +getHostName() +":" + getPortNumber();
    header += END_OF_LINE;

    header += END_OF_LINE;

    return header;
}

std::string HTTPBuilder::buildRequestLine() {
    std::string requestLine ;

    requestLine+=getMethodType() + " ";
    requestLine +=getFilePath()+ " ";
    requestLine +=DEFAULT_HTTP_VERSION;
    requestLine+=END_OF_LINE;
    return requestLine;
}

std::string HTTPBuilder::buildResponse(bool response) {
    std::string httpRequest;

    httpRequest += DEFAULT_HTTP_VERSION;
    httpRequest += " ";

    if (response) {
        httpRequest += DEFAULT_HTTP_OK;
    } else
        httpRequest += DEFAULT_HTTP_NOT_FOUND;
    httpRequest += END_OF_LINE;
    return httpRequest;
}



int HTTPBuilder::buildBody(IO *io,std::string &body) {
    //std::string body;
    //int remainingContentLength = this->getContentLength(), sendbuflen = std::min(remainingContentLength, DEFAULT_BUFLEN);
    int sendbuflen =  DEFAULT_BUFLEN;
    char buf[sendbuflen + 1];
    int result = io->open(this->getFilePath(), true);
    if(result==FILE_NOT_FOUND)return result;
    std::stringstream ss;
    while ((sendbuflen = io->readFile(buf, sendbuflen)) > 0) {
        //ss.write(buf,sendbuflen);
        //ss>>body;
        body += std::string(buf, sendbuflen);
        //remainingContentLength -= sendbuflen;
    }
    return STATUS_OK;
}


void HTTPBuilder::setHostName(std::string hostName) {
    HTTPBuilder::hostName = hostName;
}
void HTTPBuilder::setFilePath(std::string filePath) {
    HTTPBuilder::filepath = filePath;
}

void HTTPBuilder::setPortNumber(std::string portNumber) {
    HTTPBuilder::portNumber = portNumber;
}

void HTTPBuilder::setMethodType(std::string methodType) {
    HTTPBuilder::methodType = methodType;
}

const std::string &HTTPBuilder::getMethodType() const {
    return methodType;
}

const std::string &HTTPBuilder::getHostName() const {
    return hostName;
}

const std::string &HTTPBuilder::getFilePath() const {
    return filepath;
}

const std::string &HTTPBuilder::getPortNumber() const {
    return portNumber;
}

const std::string &HTTPBuilder::getResponseCode() const {
    return responseCode;
}

void HTTPBuilder::setResponseCode(std::string responseCode) {
    HTTPBuilder::responseCode = responseCode;
}

bool HTTPBuilder::isRequest() const {
    return isRequestBool;
}

void HTTPBuilder::setIsRequest(bool isRequest) {
    HTTPBuilder::isRequestBool = isRequest;
}

int HTTPBuilder::getContentLength() const {
    return contentLength;
}

void HTTPBuilder::setContentLength(int contentLength) {
    HTTPBuilder::contentLength = contentLength;
}

const std::string &HTTPBuilder::getContentType() const {
    return contentType;
}

void HTTPBuilder::setContentType(const std::string &contentType) {
    HTTPBuilder::contentType = contentType;
}

std::string HTTPBuilder::intToStr(int num) {
    std::string str;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}