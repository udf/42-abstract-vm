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

        avm.load_line(line, line_number);
        line_number++;
    }
}

void repl(AVM &avm) {
    while (std::cin.good()) {
        std::string line;

        std::cout << "> ";
        std::getline(std::cin, line);
        avm.load_line(line, 1);
        avm.step();
    }
}

auto main(int argc, char const *argv[]) -> int {
    if (argc > 2) {
        std::cerr << "Too many arguments provided." << std::endl;
        return 1;
    }

    try {
        AVM avm;

        if (argc == 1) {
            repl(avm);
            return 0;
        }

        std::ifstream fstream(argv[1]);
        read_file(fstream, avm);
        avm.run();
    } catch (AVMException &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}