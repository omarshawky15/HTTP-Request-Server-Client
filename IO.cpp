//
// Created by omara on 11/9/2021.
//

#include "IO.h"

int IO::readFile(char *buf, int bufSize) {
    if (ifstream.is_open()) {
        ifstream.read(buf, bufSize);
        if (ifstream.gcount()) {
            buf[ifstream.gcount()] = '\0';
            return ifstream.gcount();
        } else {
            ifstream.close();
            return 0;
        }
    } else return -1;
}

int IO::open(std::string filepath, bool read) {
    if (filepath[0] == '/')filepath = filepath.substr(1, filepath.size() - 1);
    if (read) {
        ifstream.open(filepath, std::ios::binary);
        if (ifstream.is_open())return STATUS_OK;
        else return FILE_NOT_FOUND;
    } else {
        ofstream.open("t_"+filepath, std::ios::binary);
        if (ofstream.is_open())return STATUS_OK;
        else return FILE_NOT_FOUND;
    }
}

bool IO::writeFile(char *buf, int bufSize) {
    if (ofstream.is_open()) {
        if (ofstream.write(buf, bufSize)) {
            return true;
        } else {
            ofstream.close();
            return false;
        }
    } else return false;

}

void IO::close(bool read) {
    if (read)ifstream.close();
    else ofstream.close();
}
std::string IO::getTime (){
    auto start = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(start);
    return std::ctime(&timeNow) ;
}
std::string IO::getMimeType(const std::string &filepath) {
    std::string szExtension;
    int fileExtentionIdx = filepath.find_last_of(".");
    if (std::string::npos != fileExtentionIdx) {
        szExtension = filepath.substr(fileExtentionIdx);
    } else return "text/plain";
    // return mime type for extension
    HKEY hKey = NULL;
    std::string szResult = "application/unknown";

    // open registry key
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension.c_str(),
                     0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // define buffer
        char szBuffer[256] = {0};
        DWORD dwBuffSize = sizeof(szBuffer);

        // get content type
        if (RegQueryValueEx(hKey, "Content Type", NULL, NULL,
                            (LPBYTE) szBuffer, &dwBuffSize) == ERROR_SUCCESS) {
            // success
            szResult = szBuffer;
        }

        // close key
        RegCloseKey(hKey);
    }

    // return result
    return szResult;
}
