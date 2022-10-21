#include "../header/getSource.h"
#include "../header/parsingSource.h"
#include "../header/ExtractParsed.h"

int main() {
    //Change url and keyword to extract
    string url = "https://example.com/";
    string keyword = "More Information";

    if (Connect(url)) {
        ParseData();
        ExtractData(keyword);
    }
    return 0;
}