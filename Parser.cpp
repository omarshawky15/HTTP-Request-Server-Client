//
// Created by omara on 11/8/2021.
//

#include "Parser.h"

HTTPBuilder *Parser::parseClientCmd(std::string cmd) {
    HTTPBuilder *builder = new HTTPBuilder();
    std::vector<std::string> parsedStr = Parser::split(cmd, " ");
    if (parsedStr.size() > 4 || parsedStr.size() < 3) {
        std::cout << "Err : Command not clear\n";
        return nullptr;
    }
    if (parsedStr.size() == 3)parsedStr.emplace_back("");
    if (parsedStr[0] == "client_get")builder->setMethodType(GET_REQUEST);
    else if (parsedStr[0] == "client_post") {
        builder->setMethodType(POST_REQUEST);
    } else {
        std::cout << "Err : Method Not Clear\n";
        return nullptr;
    }
    return builder->setFilePath(parsedStr[1])
            ->setHostName(parsedStr[2])
            ->setPortNumber(parsedStr[3]);
}

std::vector<std::string> Parser::split(std::string str, std::string delim) {
    std::vector<std::string> resultArr;
    /*int idx =0 ;
    do {
        if (str[idx] == ' ') {
            while (str[idx]  == ' ')idx++;
            if (str[idx] == 0)break;
        }
        int begin = idx;

        while (str[idx] != delim && *str)
            str++;
        std::string resultStr(begin,str);
        if(!resultStr.empty())
        resultArr.emplace_back(resultStr);
    } while (0 != *str++);*/


    size_t pos = 0;
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

HTTPBuilder *Parser::parseHeader(const std::string &header) {
    HTTPBuilder *httpBuilder = new HTTPBuilder();
    std::vector<std::string> headerLines = Parser::split(header, END_OF_LINE);
    Parser::parseResponseLine(headerLines[0], httpBuilder);
    Parser::parseHeaderContents(headerLines, httpBuilder);
    return httpBuilder;
}

void Parser::parseResponseLine(std::string &responseLine, HTTPBuilder *httpBuilder) {
    std::vector<std::string> responseLineArr = split(responseLine, " ");
    if (responseLineArr[0] == DEFAULT_HTTP_VERSION) {
        httpBuilder->setResponseCode(responseLineArr[1] + " " + responseLineArr[2]);
        httpBuilder->setIsRequest(false);
    } else {
        httpBuilder->setMethodType(responseLineArr[0]);
        httpBuilder->setFilePath(responseLineArr[1]);
        httpBuilder->setIsRequest(true);
    }
}

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
        httpBuilder->setPortNumber(splittedHost[1]);
    }

}

int Parser::strIntoInt(std::string &str) {
    std::stringstream ss;
    ss << str;
    int i;
    ss >> i;
    return i;
}
std::string Parser::intToStr(int num) {
    std::string str ;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}
