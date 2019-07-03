#pragma once
#include <string>

enum eOperandType { Int8, Int16, Int32, Float, Double };

class IOperand {
  public:
    virtual int getPrecision(void) const = 0;
    virtual eOperandType getType(void) const = 0;

    virtual IOperand const *operator+(IOperand const &rhs) const = 0;
    virtual IOperand const *operator-(IOperand const &rhs) const = 0;
    virtual IOperand const *operator*(IOperand const &rhs) const = 0;
    virtual IOperand const *operator/(IOperand const &rhs) const = 0;
    virtual IOperand const *operator%(IOperand const &rhs) const = 0;

    virtual std::string const &toString(void) const = 0;

    virtual IOperand const *create_from(int8_t value) const = 0;
    virtual IOperand const *create_from(int16_t value) const = 0;
    virtual IOperand const *create_from(int32_t value) const = 0;
    virtual IOperand const *create_from(float value) const = 0;
    virtual IOperand const *create_from(double value) const = 0;

    virtual IOperand const *clone_as(IOperand const &target) const = 0;

    virtual ~IOperand(void) {
    }
};