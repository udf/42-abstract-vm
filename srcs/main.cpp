#include <iostream>

#include "OperandFactory.hpp"
#include "exceptions.hpp"

int main(int argc, char const *argv[]) {
    (void)argc;
    (void)argv;

    OperandFactory factory;

    auto op1 = factory.createOperand(eOperandType::Float, "7");
    auto op2 = factory.createOperand(eOperandType::Int16, "5");

    auto result = *op1 + *op2;

    std::cout << result->toString() << std::endl;

    delete result;
    delete op1;
    delete op2;

    return 0;
}