#include <iostream>

#include "OperandFactory.hpp"
#include "exceptions.hpp"

int main(int argc, char const *argv[]) {
    (void)argc;
    (void)argv;
    std::cout << "sup bitches" << std::endl;

    OperandFactory factory;

    auto op = factory.createOperand(eOperandType::Double, "69");

    delete op;

    return 0;
}