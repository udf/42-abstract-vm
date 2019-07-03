#pragma once

#include <cstdint>
#include <sstream>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "exceptions.hpp"

namespace Operand {

template<typename T, eOperandType OP_TYPE>
class Operand : public IOperand {
  private:
    eOperandType calcNewType(IOperand const &rhs) {
        if (this->getPrecision() > rhs.getPrecision())
            return this->getType();
        return rhs.getType();
    }

    T value;

  public:
    typedef T type;

    Operand() {
    }

    Operand(T value) : value(value) {
    }

    Operand(std::string const &str) {
        std::istringstream stream(str);
        if (stream >> this->value)
            return;
        std::string error_info = "Invalid value for operand: \"" + str + "\"";
        throw AVMException(AVMException::Runtime, error_info);
    }

    Operand(const Operand &other) {
        *this = other;
    }

    Operand &operator=(const Operand &other) {
        this->value = other.value;
        return *this;
    }

    int getPrecision(void) const override {
        return static_cast<int>(OP_TYPE);
    }

    eOperandType getType(void) const override {
        return OP_TYPE;
    }

    IOperand const *operator+(IOperand const &rhs) const override {
    }
    IOperand const *operator-(IOperand const &rhs) const override {
    }
    IOperand const *operator*(IOperand const &rhs) const override {
    }
    IOperand const *operator/(IOperand const &rhs) const override {
    }
    IOperand const *operator%(IOperand const &rhs) const override {
    }

    std::string const &toString(void) const override {
    }

    ~Operand() override {
    }
};

using Int8 = Operand<int8_t, eOperandType::Int8>;
using Int16 = Operand<int16_t, eOperandType::Int16>;
using Int32 = Operand<int32_t, eOperandType::Int32>;
using Float = Operand<float, eOperandType::Float>;
using Double = Operand<double, eOperandType::Double>;

} // namespace Operand