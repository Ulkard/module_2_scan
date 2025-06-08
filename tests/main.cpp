#include "test_types.hpp"
#include "test_parsing.hpp"
#include "test_scan.hpp"


int main() { 
    testFixedString();
    testFormatString();

    testParsingSource();
    testParseValue();
    testParseInput();

    testScan();
}
