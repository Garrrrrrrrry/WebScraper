#ifndef EXTRACT_DATA_
#define EXTRACT_DATA_

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

using std::string;

void ExtractData(string s) {
    std::map<string, int> m;
    std::stringstream ss(s);
    string sub = "", first = "", second = "";
    while (std::getline(ss, sub, ' ')) {
        if (m.find(sub) == m.end()) {
            m.insert({sub, 1});
        }
    }
    sub = "";
    int count;
    std::ifstream fs("./web_source_files/parsedSource.txt");
    std::ofstream ofs("./web_source_files/output.txt");
    while (std::getline(fs, first, '\n')) {
        second = "";
        if (first.at(first.size() - 1) == ':') {
            second = first;
            std::getline(fs, first, '\n');
        }
        std::stringstream ss(first);
        count = 0;
        while (std::getline(ss, sub, ' ')) {
            if (m.find(sub) != m.end()) {
                ++count;
            }
        }
        //std::cout << " " << first << " " << second << " " << std::endl;
        if (count > 0) {
            if (second != "") {
                ofs << first << second;
            }
            else {
                ofs << first;
            }
        }
    }
    fs.close();
    ofs.close();
}

#endif