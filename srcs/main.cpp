#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "AVM.hpp"
#include "OperandFactory.hpp"
#include "exceptions.hpp"

auto lex(size_t line_number, std::string &line) {
    AVM::Line lexed_line;

    lexed_line.line_number = line_number;

    std::smatch matches;
    if (!std::regex_match(line, matches, std::regex("^(\\w+) ?(.+)?$")))
        throw "Failed to interpret line";

    lexed_line.instruction = matches[1];

    [&] {
        if (matches.size() != 3 || matches[2] == "")
            return;

        const std::string value = matches[2];
        if (!std::regex_match(value, matches, std::regex("^(\\w+)\\((.+?)\\).*$")))
            throw "Failed to interpret value for instruction";
        lexed_line.value_type = matches[1];
        lexed_line.value = matches[2];
    }();

    return lexed_line;
}

auto read_file(std::istream &stream) {
    std::vector<AVM::Line> lines;
    size_t line_number = 1;

    while (stream.good()) {
        std::string line;

        std::getline(stream, line);
        if (line == ";;")
            break;
        if (line.length() == 0 || line[0] == ';')
            continue;

        lines.push_back(lex(line_number, line));
        line_number++;
    }

    return lines;
}

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        std::cerr << "Too many arguments provided." << std::endl;
        return 1;
    }

    auto lines = [argc, argv] {
        if (argc == 1)
            return read_file(std::cin);
        std::ifstream fstream(argv[1]);
        return read_file(fstream);
    }();

    AVM avm(lines);

    // avm.run();

    return 0;
}