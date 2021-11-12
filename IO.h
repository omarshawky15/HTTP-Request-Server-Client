//
// Created by omara on 11/9/2021.
//

#ifndef ASSIGNMENT_1_IO_H
#define ASSIGNMENT_1_IO_H


#include <iostream>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <chrono>
#include <ctime>

class IO {
private :
    std::ifstream ifstream;
    std::ofstream ofstream;
public :
#define FILE_NOT_FOUND -1
#define STATUS_OK 0
    int readFile(char *buf, int bufSize);
    bool writeFile(char *buf, int bufSize);

    int open(std::string filepath, bool read);

    void close(bool read);

    static std::string getMimeType(const std::string &filepath);
    static std::string getTime ();
};


#endif //ASSIGNMENT_1_IO_H
