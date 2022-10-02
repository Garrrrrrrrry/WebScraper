#ifndef PARSING_FILE_
#define PARSING_FILE_

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <stack>

void ParseData() {
    std::ifstream fs;
    std::ofstream ofs;
    std::map<std::string, bool> m {
        {"title", 1},
        {"h1", 1},
        {"p", 1},
        {"a", 1}
    };
    fs.open("webSource.txt");
    ofs.open("parsedSource.txt");
    if (!fs.is_open()) {
        std::cerr << "error opening webSource.txt" << std::endl;
        exit(1);
    }
    if (!ofs.is_open()) {
        std::cerr << "error opening parsedSource.txt" << std::endl;
        exit(1);
    }
    char c;
    std::string line, tag;
    std::stack<std::string> tags;
    while (getline(fs, line, '>')) {
        if (fs.peek() == '\n') {
            fs >> c;
        }
        //find starting tag
        std::stringstream ss(line);
        std::getline(ss, tag, ' ');
        int num = tag.find('<');
        if (num != std::string::npos)
            tag = tag.substr(1);
        if (m.find(tag) != m.end()) {
            tags.push(tag);
            //if theres text after tag
            if (line.length() > tag.length()) {
                std::string write = line.substr(tag.length() + 2);
                ofs << write << ": " << std::endl;
            }
        }
        if (!(tags.empty())) {
            std::string end_tag = "</" + tags.top();
            int index = line.find(end_tag);
            //if ending tag is founds
            if (index != std::string::npos) {
                std::string write = line.substr(0, index);
                if (write != "") {
                    ofs << write << std::endl;
                }
                tags.pop();
            }
        }
    }
    fs.close();
    ofs.close();
}

#endif