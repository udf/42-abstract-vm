#include <iostream>

#include "exceptions.hpp"

int main() {
    std::cout << "sup bitches" << std::endl;

    throw AVMException<Exception::LEXER>(69, "your code gay");

    return 0;
}