#pragma once
#include <unordered_map>

#include "IOperand.hpp"
#include "Operand.hpp"

class OperandFactory {
  private:
    OperandFactory(const OperandFactory &other);
    OperandFactory &operator=(const OperandFactory &other);

    IOperand const *createInt8(std::string const &value) const;
    IOperand const *createInt16(std::string const &value) const;
    IOperand const *createInt32(std::string const &value) const;
    IOperand const *createFloat(std::string const &value) const;
    IOperand const *createDouble(std::string const &value) const;

    using createOperandFunc = decltype(&OperandFactory::createInt8);

    static const createOperandFunc createFuncs[];

    static const std::unordered_map<std::string, eOperandType> name_mapping;

  public:
    OperandFactory();
    ~OperandFactory();

    IOperand const *createOperand(
        eOperandType type,
        std::string const &value
    ) const;
    IOperand const *createOperand(
        const char *type,
        std::string const &value
    ) const;
};