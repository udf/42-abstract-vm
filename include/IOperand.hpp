#pragma once
#include <string>
#include <variant>

enum eOperandType {
    Int8,
    Int16,
    Int32,
    Float,
    Double,
    eOperandTypeMAX
};

// TODO: move this into tOperandType
static const char *sOperandName[eOperandTypeMAX] = {
    "int8",
    "int16",
    "int32",
    "float",
    "double"
};

template<eOperandType>
struct tOperandType;

template<>
struct tOperandType<Int8> { using type = int8_t; };

template<>
struct tOperandType<Int16> { using type = int16_t; };

template<>
struct tOperandType<Int32> { using type = int32_t; };

template<>
struct tOperandType<Float> { using type = float; };

template<>
struct tOperandType<Double> { using type = double; };


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