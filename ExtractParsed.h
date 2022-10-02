#ifndef EXTRACT_DATA_
#define EXTRACT_DATA_

#include <fstream>

void ExtractData() {
    std::ifstream fs("parsedSource.txt");
    std::ofstream ofs("output.txt");
}

#endif