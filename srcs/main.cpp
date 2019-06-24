#include <iostream>

#include "exceptions.hpp"

int main() {
    std::cout << "sup bitches" << std::endl;

    throw AVMException(AVMException::LEXER, "your code gay", 69);

    return 0;
}