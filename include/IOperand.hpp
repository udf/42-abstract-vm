#pragma once
#include <string>
#include <variant>
#include <utility>

enum eOperandType {
    Int8,
    Int16,
    Int32,
    Float,
    Double,
    eOperandTypeMAX
};

using eOperandTypes = std::integer_sequence<
    eOperandType,
    Int8,
    Int16,
    Int32,
    Float,
    Double
>;

template<eOperandType>
struct tOperandType;

template<>
struct tOperandType<Int8> {
    using type = int8_t;
    constexpr static const char *name = "int8";
};

template<>
struct tOperandType<Int16> {
    using type = int16_t;
    constexpr static const char *name = "int16";
};

template<>
struct tOperandType<Int32> {
    using type = int32_t;
    constexpr static const char *name = "int32";
};

template<>
struct tOperandType<Float> {
    using type = float;
    constexpr static const char *name = "float";
};

template<>
struct tOperandType<Double> {
    using type = double;
    constexpr static const char *name = "double";
};

template<typename T>
struct operand_variant_impl;

template<eOperandType... Es>
struct operand_variant_impl<std::integer_sequence<eOperandType, Es...>> {
    using type = std::variant<typename tOperandType<Es>::type...>;
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


    using operand_variant = operand_variant_impl<eOperandTypes>;

    virtual std::string toPrettyString() const = 0;

    virtual const char *getTypeName() const = 0;
    virtual operand_variant getValue() const = 0;

    virtual operand_variant convertVariant(operand_variant value) const = 0;
    virtual IOperand const *createFromVariant(operand_variant value) const = 0;
    virtual IOperand const *clone() const = 0;
};