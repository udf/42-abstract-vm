#pragma once

#include <cstdint>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "OperandOps.hpp"
#include "exceptions.hpp"

namespace Operand {

template<typename F, typename V>
V var_op(const V &x, const V &y, F f = F()) {
    return std::visit(
        [&f](auto x, auto y) -> V {
            if constexpr (std::is_same_v<decltype(x), decltype(y)>) {
                return f(x, y);
            }
            throw AVMException(
                AVMException::Runtime, "This should never happen");
        },
        x,
        y);
}

template<typename T, eOperandType ENUM_TYPE>
class Operand : public IOperand {
  private:
    Operand(const Operand &) = delete;
    Operand &operator=(const Operand &) = delete;

    T value;
    std::string str_value;

  public:
    using this_type = Operand<T, ENUM_TYPE>;

    Operand() {
    }

    Operand(T value) : value(value) {
        this->str_value = std::to_string(this->value);
    }

    Operand(std::string const &str) {
        try {
            if constexpr (std::is_same_v<T, float>) {
                this->value = std::stof(str);
            } else if constexpr (std::is_same_v<T, double>) {
                this->value = std::stod(str);
            } else {
                auto value = std::stoll(str);
                if (value > std::numeric_limits<T>::max()
                    || value < std::numeric_limits<T>::min()) {
                    throw std::out_of_range("stoll cast");
                }
                this->value = static_cast<T>(value);
            }
        } catch (const std::out_of_range &e) {
            std::string error_info =
                "Invalid value for operand: \"" + str + "\"";
            throw AVMException(AVMException::Runtime, error_info);
        }

        this->str_value = std::to_string(this->value);
    }

    int getPrecision(void) const override {
        return static_cast<int>(ENUM_TYPE);
    }

    eOperandType getType(void) const override {
        return ENUM_TYPE;
    }

    template<typename F>
    IOperand const *operand(IOperand const &rhs, F f = F()) const {
        operand_variant left = this->getValue();
        operand_variant right = rhs.getValue();
        IOperand const *factory = this;

        if (this->getPrecision() > rhs.getPrecision()) {
            right = this->convertVariant(right);
        } else if (this->getPrecision() < rhs.getPrecision()) {
            left = rhs.convertVariant(left);
            factory = &rhs;
        }
        operand_variant result = var_op(left, right, f);

        return factory->createFromVariant(result);
    }

    IOperand const *operator+(IOperand const &rhs) const override {
        return operand<std::plus<>>(rhs);
    }
    IOperand const *operator-(IOperand const &rhs) const override {
        return operand<std::minus<>>(rhs);
    }
    IOperand const *operator*(IOperand const &rhs) const override {
        return operand<std::multiplies<>>(rhs);
    }
    IOperand const *operator/(IOperand const &rhs) const override {
        return operand<std::divides<>>(rhs);
    }
    IOperand const *operator%(IOperand const &rhs) const override {
        return operand<modulus<>>(rhs);
    }

    std::string const &toString(void) const override {
        return this->str_value;
    }

    ~Operand() override {
    }

    operand_variant getValue() const override {
        return this->value;
    }

    operand_variant convertVariant(operand_variant value) const override {
        return std::visit(
            [](auto value) { return static_cast<T>(value); }, value);
    }

    IOperand const *createFromVariant(operand_variant value) const override {
        return new this_type(std::get<T>(value));
    }
};

using Int8 = Operand<int8_t, eOperandType::Int8>;
using Int16 = Operand<int16_t, eOperandType::Int16>;
using Int32 = Operand<int32_t, eOperandType::Int32>;
using Float = Operand<float, eOperandType::Float>;
using Double = Operand<double, eOperandType::Double>;

} // namespace Operand