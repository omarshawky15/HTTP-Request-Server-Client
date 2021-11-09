//
// Created by omara on 11/8/2021.
//

#ifndef ASSIGNMENT_1_PARSER_H
#define ASSIGNMENT_1_PARSER_H

#include <iostream>
#include <string.h>
#include <vector>
#include "HTTPBuilder.h"
#include <map>
class Parser {
private :
    static std::vector<std::string> split(std::string str, std::string delim);
    static void parseResponseLine(std::basic_string<char> &basicString, HTTPBuilder *httpBuilder);
public :
    HTTPBuilder * parseClientCmd(std::string cmd);

    static HTTPBuilder parseHeader(const std::string& header);

    static void parseHeaderContents(std::vector<std::string> &headerLines, HTTPBuilder *httpBuilder);

    static int strIntoInt(std::basic_string<char> &basicString);
};


#endif //ASSIGNMENT_1_PARSER_H
