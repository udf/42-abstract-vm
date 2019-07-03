#pragma once
#include <string>
#include <variant>

enum eOperandType { Int8, Int16, Int32, Float, Double };

template<typename T>
struct CastVisitor {
    template<typename U>
    T operator()(U value) {
        return static_cast<T>(value);
    }
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

    virtual std::string const &toString(void) const = 0;

    virtual ~IOperand(void) {
    }

    using operand_variant =
        std::variant<int8_t, int16_t, int32_t, float, double>;

    virtual operand_variant get_value() const = 0;

    template<typename T>
    T get_value_as() const {
        return std::visit(CastVisitor<T>(), this->get_value());
    }
};