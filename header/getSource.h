#ifndef GET_SOURCE_
#define GET_SOURCE_

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

using std::string;

#pragma comment(lib,"ws2_32.lib")

void parseUrl(string& url, string& filepath) {
    std::cout << "Url: " << url << std::endl;

    if (url.substr(0, 7) == "http://")
        url.erase(0, 7);
    if (url.substr(0, 8) == "https://")
        url.erase(0, 8);
    int index = url.find('/');
    if (index != string::npos) {
        filepath = url.substr(index);
        url = url.substr(0, index);
    }
}

int Connect(string inputUrl) {
    string url = inputUrl;
    string path = "/";
    parseUrl(url, path);
    string req = "GET " + path + " HTTP/1.0\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
    std::cout << "Request: " << req << std::endl;

    WSADATA wsaData;
    SOCKET connS;
    struct hostent* host;
    struct sockaddr_in server;
    std::ofstream ofs;

    unsigned short port_num = 80;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        std::cout << "Error: " << WSAGetLastError();
        return 0;
    }
    connS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    host = gethostbyname(url.c_str());

    server.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);

    if (connect(connS, (struct sockaddr*)&server, sizeof(server))) {
        std::cout << "Error: Connect socket issues";
        return 0;
    }
    send(connS, req.c_str(), strlen(req.c_str()), 0);

    const int bufSize = 512;
    char buffer[bufSize];
    string output;
    int currentSize;
    while (true) {
        memset(buffer, 0, bufSize);
        currentSize = recv(connS, buffer, bufSize, 0) > 0;
        if (currentSize <= 0) {
            break;
        }
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            output += buffer[i];
            i += 1;
        }
    }
    //cleanup
    closesocket(connS);
    WSACleanup();

    string end = "\r\n\r\n";
    int index = output.find(end);
    if (index != string::npos) {
        output.erase(0, index + end.size());
    }

    //std::cout << output;
    ofs.open("./web_source_files/webSource.txt");
    ofs << output;
    ofs.close();
    return 1;
}

#endif