#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "exceptions.hpp"

namespace Operand {

template<class T, class U>
struct plus {
    U operator()(const T &x, const T &y) const {
        return x + y;
    }
};

template<class T, class U>
struct minus {
    U operator()(const T &x, const T &y) const {
        return x - y;
    }
};

template<class T, class U>
struct multiply {
    U operator()(const T &x, const T &y) const {
        return x * y;
    }
};

template<class T, class U>
struct divide {
    U operator()(const T &x, const T &y) const {
        return x / y;
    }
};

template<class T, class U>
struct modulus {
    U operator()(const T &x, const T &y) const {
        return x % y;
    }
};

template<class U>
struct modulus<float, U> {
    U operator()(const float &x, const float &y) const {
        return fmodf(x, y);
    }
};

template<class U>
struct modulus<double, U> {
    U operator()(const double &x, const double &y) const {
        return fmod(x, y);
    }
};

template<typename T, eOperandType ENUM_TYPE>
class Operand : public IOperand {
  private:
    eOperandType calcNewType(IOperand const &rhs) {
        if (this->getPrecision() > rhs.getPrecision())
            return this->getType();
        return rhs.getType();
    }

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
        if (rhs.getPrecision() > this->getPrecision()) {
            std::cout << ENUM_TYPE << " delegate to " << rhs.getType()
                      << std::endl;
            return OP<const IOperand, IOperand const *>()(rhs, *this);
        }
        T rhs_value = rhs.get_value_as<T>();
        std::cout << ENUM_TYPE << " str rhs is " << rhs.toString() << std::endl;
        std::cout << ENUM_TYPE << " casted rhs is " << rhs_value << std::endl;
        T out_value = OP<T, T>()(this->value, rhs_value);
        std::cout << ENUM_TYPE << " out value is " << out_value << std::endl;
        return new Operand<T, ENUM_TYPE>(out_value);
    }

    IOperand const *operator+(IOperand const &rhs) const override {
        std::cout << ENUM_TYPE << " + op" << std::endl;
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

    ~Operand() override {
    }

    IOperand::operand_variant get_value() const override {
        std::cout << ENUM_TYPE << " returning " << +this->value << std::endl;
        return this->value;
    }
};

using Int8 = Operand<int8_t, eOperandType::Int8>;
using Int16 = Operand<int16_t, eOperandType::Int16>;
using Int32 = Operand<int32_t, eOperandType::Int32>;
using Float = Operand<float, eOperandType::Float>;
using Double = Operand<double, eOperandType::Double>;

} // namespace Operand