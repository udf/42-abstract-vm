#include "AVM.hpp"

AVM::AVM(std::vector<Line> &lines) {
    (void)(lines);

    std::string line = "push int9(8)";
    AVM::parse_line(line);
}

AVM::~AVM() {
}

void AVM::run() {
    for (auto &&instruction : this->instructions) {
        this->instr_env = &instruction.env;
        try {
            (this->*instruction.func)();
        } catch (AVMException &e) {
            e.set_line(instruction.env.line_number);
            throw;
        }
    }
}

template<typename F>
void AVM::do_binary_op(F f) {
    if (this->stack.size() < 2) {
        throw AVMException(
            Runtime,
            "binary operation with less than two values on the stack"
        );
    }

    auto right = std::move(this->stack.back());
    this->stack.pop_back();

    auto left = std::move(this->stack.back());
    this->stack.pop_back();

    auto result = f(*left, *right);
    this->stack.emplace_back(result);
}

void AVM::push() {
    this->stack.emplace_back(instr_env->arg->clone());
}

void AVM::pop() {
    if (this->stack.empty())
        throw AVMException(Runtime, "pop on empty stack");
    this->stack.pop_back();
}

void AVM::dump() {
    size_t i = 0;

    std::cout << "position: type(value) [precision]" << std::endl;
    for (auto it = this->stack.rbegin(); it != this->stack.rend(); ++it) {
        auto &item = **it;
        std::cout << i << ": " << sOperandName[item.getType()];
        std::cout << "(" << item.toString() << ")";
        std::cout << " [" << item.getPrecision() << "]";
        std::cout << std::endl;
        i++;
    }
}

void AVM::assert() {
    if (*instr_env->arg == *stack.back())
        return;
    // TODO: hint with strtype + strval of stack top/arg
    throw AVMException(Runtime, "Assertion error");
}

void AVM::add() {
    this->do_binary_op<std::plus<>>();
}

void AVM::sub() {
    this->do_binary_op<std::minus<>>();
}

void AVM::mul() {
    this->do_binary_op<std::multiplies<>>();
}

void AVM::div() {
    this->do_binary_op<std::divides<>>();
}

void AVM::mod() {
    this->do_binary_op<std::modulus<>>();
}

void AVM::print() {
    if (this->stack.size() < 1)
        throw AVMException(Runtime, "print on empty stack");
    auto &item = *this->stack.back();
    if (item.getType() != Int8)
        throw AVMException(Runtime, "print on non int8 variable");
    // TODO: figure out a way to directly use the type of an Int8 instead of respecifying it
    auto value = std::get<int8_t>(item.getValue());
    std::cout << static_cast<char>(value);
}

void AVM::exit() {
    this->exit_flag = true;
}