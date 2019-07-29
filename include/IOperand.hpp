#pragma once
#include <string>
#include <variant>

// TODO: tie enum and type together and reference those bindings instead of
// repeating the enum+type in Operand.hpp
enum eOperandType {
    Int8,
    Int16,
    Int32,
    Float,
    Double,
    eOperandTypeMAX
};

static const char *sOperandName[eOperandTypeMAX] = {
    "int8",
    "int16",
    "int32",
    "float",
    "double"
};

class IOperand {
  public:
    virtual int getPrecision(void) const = 0;
    virtual eOperandType getType(void) const = 0;

    virtual IOperand const *operator+(IOperand const &rhs) const = 0;
    virtual IOperand const *operator-(IOperand const &rhs) const = 0;
    virtual IOperand const *operator*(IOperand const &rhs) const = 0;
    virtual IOperand const *operator/(IOperand const &rhs) const = 0;
    virtual IOperand const *operator%(IOperand const &rhs) const = 0;

    virtual bool operator==(IOperand const &rhs) const = 0;

    virtual std::string const &toString(void) const = 0;

    virtual ~IOperand(void) {
    }

    using operand_variant =
        std::variant<int8_t, int16_t, int32_t, float, double>;

    virtual operand_variant getValue() const = 0;
    virtual operand_variant convertVariant(operand_variant value) const = 0;
    virtual IOperand const *createFromVariant(operand_variant value) const = 0;
    virtual IOperand const *clone() const = 0;
};