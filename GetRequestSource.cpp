#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

using std::string;

#pragma comment(lib,"ws2_32.lib")

int Connect();
HINSTANCE hInst;
WSADATA wsaData;
void mParseUrl(char* mUrl, string& serverName, string& filepath, string& filename);
SOCKET connectToServer(char* szServerName, WORD portNum);
int getHeaderLength(char* content);
char* readUrl2(char* szUrl, long& bytesReturnedOut, char** headerOut);

void ExtractData();

int main()
{
    if (Connect())
        ExtractData();
}


int Connect()
{
    const int bufLen = 1024;
    //char* szUrl = "https://example.com/";
    char* szUrl = "https://example.com/";
    long fileSize;
    char* memBuffer, * headerBuffer;
    FILE* fp;

    memBuffer = headerBuffer = NULL;

    if (WSAStartup(0x101, &wsaData) != 0)
        return -1;


    memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
    //printf("returned from readUrl\n");
    //printf("data returned:\n%s", memBuffer);
    if (fileSize != 0)
    {
        printf("Got some data:\n\n");
        fp = fopen("webSource.txt", "wb");
        fwrite(memBuffer, 1, fileSize, fp);
        fclose(fp);
        delete(memBuffer);
        delete(headerBuffer);
    }

    WSACleanup();
    return 1;
}

void mParseUrl(char* mUrl, string& serverName, string& filepath, string& filename)
{
    string::size_type n;
    string url = mUrl;

    if (url.substr(0, 7) == "http://")
        url.erase(0, 7);

    if (url.substr(0, 8) == "https://")
        url.erase(0, 8);

    n = url.find('/');
    if (n != string::npos)
    {
        serverName = url.substr(0, n);
        filepath = url.substr(n);
        n = filepath.rfind('/');
        filename = filepath.substr(n + 1);
    }

    else
    {
        serverName = url;
        filepath = "/";
        filename = "";
    }
}

SOCKET connectToServer(char* szServerName, WORD portNum)
{
    struct hostent* hp;
    unsigned int addr;
    struct sockaddr_in server;
    SOCKET conn;

    conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn == INVALID_SOCKET)
        return NULL;

    if (inet_addr(szServerName) == INADDR_NONE)
    {
        hp = gethostbyname(szServerName);
    }
    else
    {
        addr = inet_addr(szServerName);
        hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if (hp == NULL)
    {
        closesocket(conn);
        return NULL;
    }

    server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);
    if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
    {
        closesocket(conn);
        return NULL;
    }
    return conn;
}

int getHeaderLength(char* content)
{
    const char* srchStr1 = "\r\n\r\n", * srchStr2 = "\n\r\n\r";
    char* findPos;
    int ofset = -1;

    findPos = strstr(content, srchStr1);
    if (findPos != NULL)
    {
        ofset = findPos - content;
        ofset += strlen(srchStr1);
    }

    else
    {
        findPos = strstr(content, srchStr2);
        if (findPos != NULL)
        {
            ofset = findPos - content;
            ofset += strlen(srchStr2);
        }
    }
    return ofset;
}

char* readUrl2(char* szUrl, long& bytesReturnedOut, char** headerOut)
{
    const int bufSize = 512;
    char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
    char* tmpResult = NULL, * result;
    SOCKET conn;
    string server, filepath, filename;
    long totalBytesRead, thisReadSize, headerLen;

    mParseUrl(szUrl, server, filepath, filename);

    ///////////// step 1, connect //////////////////////
    conn = connectToServer((char*)server.c_str(), 80);

    ///////////// step 2, send GET request /////////////
    sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
    strcpy(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", server.c_str());
    strcat(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");
    send(conn, sendBuffer, strlen(sendBuffer), 0);

    //    SetWindowText(edit3Hwnd, sendBuffer);
    printf("Buffer being sent:\n%s", sendBuffer);

    ///////////// step 3 - get received bytes ////////////////
    // Receive until the peer closes the connection
    totalBytesRead = 0;
    while (1)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv(conn, readBuffer, bufSize, 0);

        if (thisReadSize <= 0)
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

        memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    headerLen = getHeaderLength(tmpResult);
    long contenLen = totalBytesRead - headerLen;
    result = new char[contenLen + 1];
    memcpy(result, tmpResult + headerLen, contenLen);
    result[contenLen] = 0x0;
    char* myTmp;

    myTmp = new char[headerLen + 1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contenLen;
    closesocket(conn);
    return(result);
}

void ExtractData()
{
    std::ifstream fs;
    std::map<std::string, bool> m{
        {"title", 1},
        {"h1", 1},
        {"p", 1},
    };
    string output;

    fs.open("webSource.txt");

    if (!fs.is_open())
    {
        std::cerr << "error opening file" << std::endl;
        exit(1);
    }
    else
    {
        std::string s, tag, end;
        do
        {
            fs.ignore(256, '<');
            getline(fs, s, '>');
            std::stringstream ss(s);
            tag = "";
            getline(ss, tag, ' ');
            try {
                if (m.at(tag))
                {
                    output, end = "";
                    while (end != "/" + tag)
                    {
                        getline(fs, output, '<');
                        getline(fs, end, '>');
                        if (end != ("/" + tag))
                        {
                            output += end;
                        }
                    }
                    std::cout <<  output << std::endl;
                }
            }
            catch (const std::out_of_range&) 
            {
                fs.ignore(256, '<');
            }
        } while (fs.ignore(256, '<'));
    }
}