#include "AVM.hpp"

AVM::AVM() {
}

AVM::~AVM() {
}

void AVM::load_line(std::string &line, size_t line_number) {
    try {
        auto instruction = this->parse_line(line);
        if (!instruction)
            return;
        this->instructions.push_back(std::move(instruction.value()));
        this->instructions.back().line_number = line_number;
    } catch (AVMException &e) {
        e.set_line(line_number);
        throw;
    }
}

void AVM::step() {
    if (this->instruction_ptr > this->instructions.size())
        throw AVMException(Runtime, "Unexpected end of instructions (missing exit?)");

    auto &instruction = this->instructions.at(this->instruction_ptr);

    this->instruction_ptr++;
    this->instr_arg = instruction.arg.get();
    try {
        (this->*instruction.func)();
    } catch (AVMException &e) {
        e.set_line(instruction.line_number);
        throw;
    }
}

void AVM::run() {
    while (this->running) {
        this->step();
    }
}

void AVM::_assert(bool condition, std::string info) {
    if (!condition) {
        throw AVMException(Runtime, info);
    }
}

template<typename F>
void AVM::do_binary_op(F f) {
    _assert(
        this->stack.size() >= 2,
        "binary operation with less than two values on the stack"
    );

    auto right = std::move(this->stack.back());
    this->stack.pop_back();

    auto left = std::move(this->stack.back());
    this->stack.pop_back();

    try {
        auto result = f(*left, *right);
        this->stack.emplace_back(result);
    } catch (AVMException &e) {
        e.set_type(Runtime);
        throw;
    }
}

void AVM::push() {
    this->stack.emplace_back(instr_arg->clone());
}

void AVM::pop() {
    _assert(!this->stack.empty(), "pop on empty stack");
    this->stack.pop_back();
}

void AVM::dump() {
    size_t i = 0;

    std::cout << "position: type(value) [precision]" << std::endl;
    for (auto it = this->stack.rbegin(); it != this->stack.rend(); ++it) {
        auto &item = **it;
        std::cout << i << ": " << item.getTypeName();
        std::cout << "(" << item.toString() << ")";
        std::cout << " [" << item.getPrecision() << "]";
        std::cout << std::endl;
        i++;
    }
}

void AVM::assert() {
    if (*instr_arg == *stack.back())
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
    _assert(this->stack.size() >= 1, "print on empty stack");
    auto &item = *this->stack.back();
    _assert(item.getType() == Int8, "print on non int8 variable");
    auto value = std::get<tOperandType<Int8>::type>(item.getValue());
    std::cout << static_cast<char>(value);
}

void AVM::exit() {
    this->running = false;
}