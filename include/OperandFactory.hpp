#pragma once
#include <unordered_map>

#include "IOperand.hpp"
#include "Operand.hpp"

class OperandFactory {
  private:
    OperandFactory(const OperandFactory &other);
    auto operator=(const OperandFactory &other) -> OperandFactory &;

    auto createInt8(std::string const &value) const -> IOperand const *;
    auto createInt16(std::string const &value) const -> IOperand const *;
    auto createInt32(std::string const &value) const -> IOperand const *;
    auto createFloat(std::string const &value) const -> IOperand const *;
    auto createDouble(std::string const &value) const -> IOperand const *;

    using createOperandFunc = decltype(&OperandFactory::createInt8);

    static const createOperandFunc createFuncs[];

    static const std::unordered_map<std::string, eOperandType> name_mapping;

  public:
    OperandFactory();
    ~OperandFactory();

    auto createOperand(
        eOperandType type,
        std::string const &value
    ) const -> IOperand const *;
    auto createOperand(
        const char *type,
        std::string const &value
    ) const -> IOperand const *;
};