#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "AVM.hpp"
#include "OperandFactory.hpp"
#include "AVMException.hpp"

auto read_file(std::istream &stream, AVM &avm) {
    size_t line_number = 1;

    while (stream.good()) {
        std::string line;

        std::getline(stream, line);
        if (line == ";;")
            break;

        avm.load_line(line);
        line_number++;
    }
}

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        std::cerr << "Too many arguments provided." << std::endl;
        return 1;
    }

    try {
        AVM avm;

        if (argc == 1) {
            read_file(std::cin, avm);
        } else {
            std::ifstream fstream(argv[1]);
            read_file(fstream, avm);
        }

        avm.run();
    } catch (AVMException &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}