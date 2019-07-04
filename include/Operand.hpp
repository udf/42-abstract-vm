#pragma once

#include <cstdint>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "OperandOps.hpp"
#include "exceptions.hpp"

namespace Operand {

template<typename T, eOperandType ENUM_TYPE>
class Operand : public IOperand {
  private:
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

    Operand(const Operand &other) {
        *this = other;
    }

    Operand &operator=(const Operand &other) {
        this->value = other.value;
        this->str_value = other.str_value;
        return *this;
    }

    int getPrecision(void) const override {
        return static_cast<int>(ENUM_TYPE);
    }

    eOperandType getType(void) const override {
        return ENUM_TYPE;
    }

    template<template<class, class> class OP>
    IOperand const *operand(IOperand const &rhs) const {
        IOperand const *lhs_ptr = this;
        IOperand const *rhs_ptr = &rhs;

        if (rhs_ptr->getPrecision() > lhs_ptr->getPrecision()) {
            lhs_ptr = rhs.create_from(this->value);
            auto ret = OP<const IOperand, IOperand const *>()(*lhs_ptr, *this);
            delete lhs_ptr;
            return ret;
        } else if (lhs_ptr->getPrecision() > rhs_ptr->getPrecision()) {
            rhs_ptr = rhs.clone_as(*lhs_ptr);
        }

        auto casted_rhs = static_cast<decltype(this)>(rhs_ptr);
        auto ret = new this_type(OP<T, T>()(this->value, casted_rhs->value));

        if (rhs_ptr != &rhs)
            delete rhs_ptr;

        return ret;
    }

    IOperand const *operator+(IOperand const &rhs) const override {
        return operand<plus>(rhs);
    }
    IOperand const *operator-(IOperand const &rhs) const override {
        return operand<minus>(rhs);
    }
    IOperand const *operator*(IOperand const &rhs) const override {
        return operand<multiply>(rhs);
    }
    IOperand const *operator/(IOperand const &rhs) const override {
        return operand<divide>(rhs);
    }
    IOperand const *operator%(IOperand const &rhs) const override {
        return operand<modulus>(rhs);
    }

    std::string const &toString(void) const override {
        return this->str_value;
    }

    IOperand const *create_from(int8_t value) const override {
        return new this_type(static_cast<T>(value));
    }
    IOperand const *create_from(int16_t value) const override {
        return new this_type(static_cast<T>(value));
    }
    IOperand const *create_from(int32_t value) const override {
        return new this_type(static_cast<T>(value));
    }
    IOperand const *create_from(float value) const override {
        return new this_type(static_cast<T>(value));
    }
    IOperand const *create_from(double value) const override {
        return new this_type(static_cast<T>(value));
    }

    IOperand const *clone_as(IOperand const &target) const override {
        return target.create_from(this->value);
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