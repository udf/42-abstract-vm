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

IOperand const *OperandFactory::createInt8(std::string const &value) const {
    return new Operand::Int8(value);
}

IOperand const *OperandFactory::createInt16(std::string const &value) const {
    return new Operand::Int16(value);
}

IOperand const *OperandFactory::createInt32(std::string const &value) const {
    return new Operand::Int32(value);
}

IOperand const *OperandFactory::createFloat(std::string const &value) const {
    return new Operand::Float(value);
}

IOperand const *OperandFactory::createDouble(std::string const &value) const {
    return new Operand::Double(value);
}
