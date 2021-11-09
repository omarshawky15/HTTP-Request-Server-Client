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
        }else {
            ifstream.close();
            return 0;
        }
    } else return -1;
}

void IO::open(std::string filepath, bool read) {
    if(filepath[0] =='/')filepath = filepath.substr(1,filepath.size()-1);
    if (read) {
        ifstream.open(filepath);
    } else {
            ofstream.open(filepath);
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

int IO::checkFile(std::string filepath) {
    //std::replace( filepath.begin(), filepath.end(), '/', '_');
    if(filepath[0] =='/')filepath = filepath.substr(1,filepath.size()-1);
    int contentLength = 0 ;
    std::ifstream tempIfstream;
    tempIfstream.open(filepath);
    if (tempIfstream.is_open()) {
        std::string lineStr;
        while (getline(tempIfstream, lineStr)) {
            contentLength += lineStr.size();
        }
    } else
        return -1;
    tempIfstream.close();
    return contentLength;
}
void IO::close(bool read) {
    if(read)ifstream.close();
    else ofstream.close();
}

std ::string IO::GetMimeType(const std ::string &szExtension)
{
    // return mime type for extension
    HKEY hKey = NULL;
    std ::string szResult = "application/unknown";

    // open registry key
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension.c_str(),
                     0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        // define buffer
        char szBuffer[256] = {0};
        DWORD dwBuffSize = sizeof(szBuffer);

        // get content type
        if (RegQueryValueEx(hKey, "Content Type", NULL, NULL,
                            (LPBYTE)szBuffer, &dwBuffSize) == ERROR_SUCCESS)
        {
            // success
            szResult = szBuffer;
        }

        // close key
        RegCloseKey(hKey);
    }

    // return result
    return szResult;
}