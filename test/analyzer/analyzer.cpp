#include "analyzer.h"
#include "analyzer_MIDI.h"
#include <stdio.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

USING_NAMESPACE_ANALYZER

int main(int, char**) {
    MIDIAnalyzer analyzer;
    analyzer.setup();

    std::ifstream csvFile("../bytes.csv");
    std::cout << "File open" << std::endl;
    std::string line = "";
    while (std::getline(csvFile, line))
    {
        uint8_t n = std::stoi(line, nullptr, 16);
        analyzer.process(n);
    }
}
