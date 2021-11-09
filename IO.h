//
// Created by omara on 11/9/2021.
//

#ifndef ASSIGNMENT_1_IO_H
#define ASSIGNMENT_1_IO_H


#include <iostream>
#include <fstream>
#include <algorithm>
#include <Windows.h>

class IO {
private :
    std::ifstream ifstream;
    std::ofstream ofstream;
public :
    int readFile(char *buf, int bufSize);
    bool writeFile(char *buf, int bufSize);

    void open(std::string filepath, bool read);

    void close(bool read);

    static int checkFile(std::string filepath);

    static std::string GetMimeType(const std::string &szExtension);
};


#endif //ASSIGNMENT_1_IO_H
