#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "AVM.hpp"
#include "OperandFactory.hpp"
#include "AVMException.hpp"

auto read_file(
    AVM &avm,
    std::istream &stream,
    bool is_stdin = false
) {
    size_t line_number = 1;
    std::vector<std::string> errors;

    while (stream.good()) {
        std::string line;

        std::getline(stream, line);
        if (is_stdin && line == ";;")
            break;

        try {
            avm.load_line(line, line_number);
        } catch (const AVMException &e) {
            if (is_stdin)
                throw;
            errors.push_back(e.what());
        }
        line_number++;
    }

    if (!errors.empty()) {
        for (auto &what : errors) {
            std::cerr << what << std::endl;
        }
        throw AVMException(Parser, "One or more errors occurred");
    }
}

void repl() {
    AVM avm;

    while (true) {
        std::string line;

        std::cout << "> ";
        std::getline(std::cin, line);
        if (!std::cin.good())
            break;
        try {
            avm.load_line(line, 1);
            avm.step();
        } catch (const AVMException &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

auto main(int argc, char const *argv[]) -> int {
    if (argc > 2) {
        std::cerr << "Too many arguments provided." << std::endl;
        return 1;
    }

    if (argc == 2 && std::strcmp(argv[1], "-i") == 0) {
        repl();
        return 0;
    }

    try {
        AVM avm;

        if (argc == 1) {
            read_file(avm, std::cin, true);
        } else {
            std::ifstream fstream(argv[1]);
            read_file(avm, fstream);
        }

        avm.run();
    } catch (AVMException &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}