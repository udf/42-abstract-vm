#include "OperandFactory.hpp"

const OperandFactory::createOperandFunc OperandFactory::createFuncs[] = {
    &OperandFactory::createInt8,
    &OperandFactory::createInt16,
    &OperandFactory::createInt32,
    &OperandFactory::createFloat,
    &OperandFactory::createDouble,
};

decltype(OperandFactory::name_mapping) OperandFactory::name_mapping({
    {"int8", Int8},
    {"int16", Int16},
    {"int32", Int32},
    {"float", Float},
    {"double", Double},
});

OperandFactory::OperandFactory() {
}

OperandFactory::~OperandFactory() {
}

auto OperandFactory::createOperand(
    eOperandType type,
    std::string const &value
) const -> IOperand const * {
    return (this->*createFuncs[type])(value);
}

auto OperandFactory::createOperand(
    const char *type,
    std::string const &value
) const -> IOperand const * {
    auto it = name_mapping.find(type);
    if (it == name_mapping.end()) {
        throw AVM::Exception("Unknown operand type:")
            .set_hint(type);
    }
    return this->createOperand((*it).second, value);
}

auto OperandFactory::createInt8(std::string const &value) const
    -> IOperand const *
{
    return new Operand::Int8(value);
}

auto OperandFactory::createInt16(std::string const &value) const
    -> IOperand const *
{
    return new Operand::Int16(value);
}

auto OperandFactory::createInt32(std::string const &value) const
    -> IOperand const *
{
    return new Operand::Int32(value);
}

auto OperandFactory::createFloat(std::string const &value) const
    -> IOperand const *
{
    return new Operand::Float(value);
}

auto OperandFactory::createDouble(std::string const &value) const
    -> IOperand const *
{
    return new Operand::Double(value);
}
