#include "OperandFactory.hpp"

const OperandFactory::createOperandFunc OperandFactory::createFuncs[] = {
    &OperandFactory::createInt8,
    &OperandFactory::createInt16,
    &OperandFactory::createInt32,
    &OperandFactory::createFloat,
    &OperandFactory::createDouble,
};

OperandFactory::OperandFactory() {
}

OperandFactory::~OperandFactory() {
}

IOperand const *OperandFactory::createOperand(
    eOperandType type,
    std::string const &value) const {
    return createFuncs[type](*this, value);
}