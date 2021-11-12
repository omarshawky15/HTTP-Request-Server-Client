//
// Created by omara on 11/8/2021.
//

#include "Parser.h"
/**
   *  @param  cmd  command read from input.txt to be parsed.
   *  @return HTTP object build based on the client command that contains hostname, port number and filepath
   *
   *  parses clients commands from input.txt and builds HTTP object from it
  */
HTTPBuilder *Parser::parseClientCmd(std::string cmd) {
    HTTPBuilder *builder = new HTTPBuilder();
    std::vector<std::string> parsedStr = Parser::split(cmd, " ");
    if (parsedStr.size() > 4 || parsedStr.size() < 2) {
        std::cout << "Err : Command not clear\n";
        return nullptr;
    }
    //if (parsedStr.size() == 3)parsedStr.emplace_back("");
    if (parsedStr[0] == "client_get")builder->setMethodType(GET_REQUEST);
    else if (parsedStr[0] == "client_post") {
        builder->setMethodType(POST_REQUEST);
    } else {
        std::cout << "Err : Method Not Clear\n";
        return nullptr;
    }
    builder->setIsRequest(true);
    builder->setFilePath(parsedStr[1]);
    if(parsedStr.size()>2)builder->setHostName(parsedStr[2]);
    if(parsedStr.size()>3)builder->setPortNumber(parsedStr[3]);
    return builder;
}
/**
   *  @param  str  string to be split.
   *  @param  delim  delimiter to state where to split two strings.
   *  @return vector of strings seperated after splitting str
   *
   *  splits a string into multiple strings with delim as seperator
  */
std::vector<std::string> Parser::split(std::string str, std::string delim) {
    std::vector<std::string> resultArr;

    size_t pos ;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        str.erase(0, pos + delim.length());
        if (!token.empty())
            resultArr.emplace_back(token);
    }
    if (!str.empty())
        resultArr.emplace_back(str);
    return resultArr;
}
/**
   *  @param  header  string to be split.
   *  @return HTTP object that contains data extracted from passed header
   *
   *  parse header received from recv and split it to multiple lines then store each field in HTTP object
  */
HTTPBuilder *Parser::parseHeader(const std::string &header) {
    HTTPBuilder *httpBuilder = new HTTPBuilder();
    std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
    Parser::parseResponseLine(headerLines[0], httpBuilder);
    Parser::parseHeaderContents(headerLines, httpBuilder);
    return httpBuilder;
}
/**
   *  @param  httpBuilder  HTTP object to store extracted information in.
   *  @param  RRLine  string representing first line of header to be parsed.
   *
   *  parse first line of header to indicate wether it's a response or request and which type of request received
  */
//
void Parser::parseResponseLine(std::string &RRLine, HTTPBuilder *httpBuilder) {
    std::vector<std::string> responseLineArr = split(RRLine, " ");
    if (responseLineArr[0] == DEFAULT_HTTP_VERSION) {
        std::vector<std::string> statusCode = split(responseLineArr[2],END_OF_LINE);
        httpBuilder->setResponseCode(responseLineArr[1] + " " + statusCode[0]);
        httpBuilder->setIsRequest(false);
    } else {
        httpBuilder->setMethodType(responseLineArr[0]);
        httpBuilder->setFilePath(responseLineArr[1]);
        httpBuilder->setIsRequest(true);
    }
}
/**
   *  @param  headerLines  splitted lines of header each representing one field and its value.
   *  @param  httpBuilder  HTTP object to store extracted data in.
   *
   *  split header into key :value using a " " delimeter and store it in map
   *  then for the values I'm searching for, check it in map and if it exists store in HTTP object
  */

void Parser::parseHeaderContents(std::vector<std::string> &headerLines, HTTPBuilder *httpBuilder) {
    std::map<std::string, std::string> headerMap;
    for (int i = 1; i < headerLines.size(); i++) {
        std::vector<std::string> splittedLine = split(headerLines[i], " ");
        headerMap.insert(std::make_pair(splittedLine[0], splittedLine[1]));
    }

    if (headerMap.count(CONTENT_LENGTH_FIELD))
        httpBuilder->setContentLength(Parser::strIntoInt(headerMap[CONTENT_LENGTH_FIELD]));
    if (headerMap.count(CONTENT_TYPE_FIELD))
        httpBuilder->setContentType(headerMap[CONTENT_LENGTH_FIELD]);
    if (headerMap.count(HOST_NAME_FIELD)) {
        std::vector<std::string> splittedHost = split(headerMap[HOST_NAME_FIELD], ":");
        httpBuilder->setHostName(splittedHost[0]);
        if(splittedHost.size()>1)httpBuilder->setPortNumber(splittedHost[1]);
        else httpBuilder->setPortNumber(DEFAULT_PORT);
    }

}
/**
   *  @param  str  string to be translated to int.
   *  @return int integer representation of str
   *
   *  transforms str to integer number
  */
int Parser::strIntoInt(std::string &str) {
    std::stringstream ss;
    ss << str;
    int i;
    ss >> i;
    return i;
}
/**
   *  @param  num  number to be translated to string.
   *  @return string string representation of integer.
   *
   *  transforms integer number to str
  */
std::string Parser::intToStr(int num) {
    std::string str;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}
