//
// Created by omara on 11/8/2021.
//

#include "HTTPBuilder.h"

HTTPBuilder *HTTPBuilder::setHostName(std::string hostName) {
    HTTPBuilder::hostName = hostName;
    return this;
}

HTTPBuilder *HTTPBuilder::setFilePath(std::string filePath) {
    HTTPBuilder::filePath = filePath;
    return this;
}

HTTPBuilder *HTTPBuilder::setPortNumber(std::string portNumber) {
    HTTPBuilder::portNumber = portNumber;
    return this;
}

HTTPBuilder *HTTPBuilder::setMethodType(std::string methodType) {
    HTTPBuilder::methodType = methodType;
    return this;
}

std::string HTTPBuilder::buildRequest() {
    std::string httpRequest;


    httpRequest += methodType + " /";

    httpRequest += filePath + " ";
    httpRequest += DEFAULT_HTTP_VERSION;
    httpRequest += END_OF_LINE;

    httpRequest += HOST_NAME_FIELD;
    httpRequest += " " + hostName;
    httpRequest += ":"+portNumber;
    httpRequest += END_OF_LINE;

    if (HTTPBuilder::methodType == "POST") {
        if (!HTTPBuilder::checkFile()) {
            std::cout << "Err : File Not Found\n";
            return "";
        }
        myFileStream.open(filePath);
        httpRequest += CONTENT_LENGTH_FIELD;
        httpRequest += " " + std::to_string(contentLength);
        httpRequest += END_OF_LINE;

        httpRequest += CONTENT_TYPE_FIELD;
        int fileExtentionIdx = filePath.find_last_of(".");
        if (std::string::npos != fileExtentionIdx)
            httpRequest += " " + filePath.substr(fileExtentionIdx + 1);
        httpRequest += END_OF_LINE;
    }

    httpRequest += END_OF_LINE;
    return httpRequest;
}

std::string HTTPBuilder::buildResponse(bool response) {
    std::string httpRequest = "";

    httpRequest += DEFAULT_HTTP_VERSION;
    httpRequest += END_OF_LINE;

    if(response) {
        httpRequest += DEFAULT_HTTP_OK;
    }else
        httpRequest += DEFAULT_HTTP_NOT_FOUND;
    httpRequest += END_OF_LINE;

    if (HTTPBuilder::methodType == "POST") {
        if (!HTTPBuilder::checkFile()) {
            std::cout << "Err : File Not Found\n";
            return "";
        }
        myFileStream.open(filePath);
        httpRequest += CONTENT_LENGTH_FIELD;
        httpRequest += " " + std::to_string(contentLength);
        httpRequest += END_OF_LINE;

        httpRequest += CONTENT_TYPE_FIELD;
        int fileExtentionIdx = filePath.find_last_of(".");
        if (std::string::npos != fileExtentionIdx)
            httpRequest += " " + filePath.substr(fileExtentionIdx + 1);
        httpRequest += END_OF_LINE;
    }

    httpRequest += END_OF_LINE;
    return httpRequest;
}

char *HTTPBuilder::readFile(char *buf, int bufSize) {
    if (myFileStream.is_open()) {
        if (myFileStream.read(buf, bufSize)) {
            return buf;
        } else if (myFileStream.gcount() > 0) {
            if (myFileStream.gcount() != bufSize)
                buf[myFileStream.gcount()] = '\0';
            return buf;
        } else {
            myFileStream.close();
            return nullptr;
        }
    } else return nullptr;
}

HTTPBuilder::HTTPBuilder() {
    contentLength = 0;
    fileData = "";
    methodType =GET_REQUEST;
    hostName = DEFAULT_HOST;
    portNumber = DEFAULT_PORT;
}

bool HTTPBuilder::checkFile() {
    std::ifstream myfile;
    myfile.open(filePath);
    if (myfile.is_open()) {
        std::string lineStr;
        while (getline(myfile, lineStr)) {
            contentLength += lineStr.size();
        }
    } else
        return false;
    myfile.close();
    return true;
}

const std::string &HTTPBuilder::getMethodType() const {
    return methodType;
}

const std::string &HTTPBuilder::getHostName() const {
    return hostName;
}

const std::string &HTTPBuilder::getFilePath() const {
    return filePath;
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
