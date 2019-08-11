#pragma once

#include <type_traits>
#include <cstdint>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "OperandOps.hpp"
#include "AVMException.hpp"

namespace Operand {

template<eOperandType ENUM_TYPE>
class Operand;

using Int8 = Operand<eOperandType::Int8>;
using Int16 = Operand<eOperandType::Int16>;
using Int32 = Operand<eOperandType::Int32>;
using Float = Operand<eOperandType::Float>;
using Double = Operand<eOperandType::Double>;

template<typename T, typename... Ts>
constexpr auto is_one_of_vt() -> bool {
    return std::disjunction_v<std::is_same<T, typename Ts::T>...>;
}

template<typename F, typename V>
auto var_op(const V &x, const V &y, F f = F()) -> V {
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
                    throw AVM::Exception("Overflow");
                if (result < std::numeric_limits<x_t>::min())
                    throw AVM::Exception("Underflow");
                return static_cast<x_t>(result);
            }
            return f(x, y);
        }
        throw AVM::Exception("This should never happen");
    };
    return std::visit(visitor, x, y);
}

template<eOperandType ENUM_TYPE>
class Operand : public IOperand {
  public:
    using this_type = Operand<ENUM_TYPE>;
    using T = typename tOperandType<ENUM_TYPE>::type;

  private:
    Operand(const Operand &) = delete;
    auto operator=(const Operand &) -> Operand & = delete;

    T value;
    std::string str_value;

  public:
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
            throw AVM::Exception("Invalid value for operand:")
                .set_hint(str);
        }

        this->str_value = std::to_string(this->value);
    }

    auto getPrecision(void) const -> int override {
        return static_cast<int>(ENUM_TYPE);
    }

    auto getType(void) const -> eOperandType override {
        return ENUM_TYPE;
    }

    template<typename F>
    auto operand(IOperand const &rhs, F f = F()) const -> IOperand const * {
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

    auto operator+(IOperand const &rhs) const -> IOperand const * override {
        return operand<std::plus<>>(rhs);
    }
    auto operator-(IOperand const &rhs) const -> IOperand const * override {
        return operand<std::minus<>>(rhs);
    }
    auto operator*(IOperand const &rhs) const -> IOperand const * override {
        return operand<std::multiplies<>>(rhs);
    }
    auto operator/(IOperand const &rhs) const -> IOperand const * override {
        return operand<divides<>>(rhs);
    }
    auto operator%(IOperand const &rhs) const -> IOperand const * override {
        return operand<modulus<>>(rhs);
    }

    auto operator==(IOperand const &rhs) const -> bool override {
        if (this->getType() != rhs.getType())
            return false;
        this_type const &rhs_t = static_cast<this_type const &>(rhs);
        return this->value == rhs_t.value;
    }

    auto toString(void) const -> std::string const & override {
        return this->str_value;
    }

    ~Operand() override {
    }

    auto getTypeName() const -> const char * override {
        return tOperandType<ENUM_TYPE>::name;
    }

    auto toPrettyString() const -> std::string override {
        std::string ret = this->getTypeName();
        ret += '(';
        ret += this->toString();
        ret += ')';
        return ret;
    }

    auto getValue() const -> operand_variant override {
        return this->value;
    }

    auto convertVariant(operand_variant value) const
        -> operand_variant override
    {
        return std::visit(
            [](auto value) {
                return static_cast<T>(value);
            },
            value
        );
    }

    auto createFromVariant(operand_variant value) const
        -> IOperand const * override
    {
        return new this_type(std::get<T>(value));
    }

    auto clone() const -> IOperand const * override {
        return new this_type(this->value);
    }
};

} // namespace Operand