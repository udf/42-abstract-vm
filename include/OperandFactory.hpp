#pragma once
#include <functional>

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

    using createOperandFunc = std::function<
        IOperand const *(const OperandFactory &, std::string const &)>;

    static const createOperandFunc createFuncs[];

  public:
    OperandFactory();
    ~OperandFactory();

    IOperand const *
    createOperand(eOperandType type, std::string const &value) const;
};