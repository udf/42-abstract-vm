#pragma once

#include <type_traits>
#include <cstdint>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "OperandOps.hpp"
#include "AVMException.hpp"

namespace Operand {

template<typename T, eOperandType ENUM_TYPE>
class Operand;

using Int8 = Operand<int8_t, eOperandType::Int8>;
using Int16 = Operand<int16_t, eOperandType::Int16>;
using Int32 = Operand<int32_t, eOperandType::Int32>;
using Float = Operand<float, eOperandType::Float>;
using Double = Operand<double, eOperandType::Double>;

template<typename T, typename... Ts>
constexpr bool is_one_of_vt() {
    return std::disjunction_v<std::is_same<T, typename Ts::value_type>...>;
}

template<typename F, typename V>
V var_op(const V &x, const V &y, F f = F()) {
    const auto visitor = [&f](auto x, auto y) -> V {
        using x_t = decltype(x);

        if constexpr (std::is_same_v<x_t, decltype(y)>) {
            // Hack time: if our operands are an integer type, then do
            // the calculation as int64. This lets us do over/underflow
            // checks easily
            if constexpr (is_one_of_vt<x_t, Int8, Int16, Int32>()) {
                int64_t result = f(
                    static_cast<int64_t>(x),
                    static_cast<int64_t>(y)
                );
                if (result > std::numeric_limits<x_t>::max())
                    throw AVMException("Overflow");
                if (result < std::numeric_limits<x_t>::min())
                    throw AVMException("Underflow");
                return static_cast<x_t>(result);
            }
            return f(x, y);
        }
        throw AVMException("This should never happen");
    };
    return std::visit(visitor, x, y);
}

template<typename T, eOperandType ENUM_TYPE>
class Operand : public IOperand {
  private:
    Operand(const Operand &) = delete;
    Operand &operator=(const Operand &) = delete;

    T value;
    std::string str_value;

  public:
    using value_type = T;
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
            throw AVMException("Invalid value for operand:")
                .set_hint(str);
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
            [](auto value) {
                return static_cast<T>(value);
            },
            value
        );
    }

    IOperand const *createFromVariant(operand_variant value) const override {
        return new this_type(std::get<T>(value));
    }

    IOperand const *clone() const override {
        return new this_type(this->value);
    }
};

} // namespace Operand