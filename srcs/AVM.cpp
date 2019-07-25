#include "AVM.hpp"

decltype(AVM::instr_defs) AVM::instr_defs({
    {"push", {&AVM::push, true}},
    {"pop", {&AVM::pop, false}},
    {"dump", {&AVM::dump, false}},
    {"assert", {&AVM::assert, false}},
    {"add", {&AVM::add, false}},
    {"sub", {&AVM::sub, false}},
    {"mul", {&AVM::mul, false}},
    {"div", {&AVM::div, false}},
    {"mod", {&AVM::mod, false}},
    {"print", {&AVM::print, false}},
    {"exit", {&AVM::exit, false}},
});

decltype(AVM::factory) AVM::factory = OperandFactory();

AVM::AVM(std::vector<Line> &lines) {
    // Parse the lines into memes
    for (auto &&line : lines) {
        auto it = instr_defs.find(line.instruction.c_str());
        if (it == instr_defs.end()) {
            throw AVMException(Parser, "Unknown instruction")
                .set_line(line.line_number);
        }

        const InstrDef &func = it->second;
        bool line_has_arg = !line.value.empty();

        if (func.needs_arg != line_has_arg) {
            const char *info = func.needs_arg
                ? "Instruction needs an argument but none was provided"
                : "Instruction takes no arguments but one was provided";
            throw AVMException(Parser, info).set_line(line.line_number);
        }

        if (!func.needs_arg) {
            // TODO: make a constructor and use emplace_back
            this->instructions.push_back({func.func, nullptr});
            continue;
        }

        // TODO: rethrow internal exception
        operand_uptr arg = operand_uptr(this->factory.createOperand(
            line.value_type.c_str(),
            line.value
        ));
        this->instructions.push_back({func.func, std::move(arg)});
    }
}

AVM::~AVM() {
}

void AVM::run() {
    // TODO: store line number in instruction struct and rethrow with line number
    for (auto &&instruction : this->instructions) {
        this->instr_arg = instruction.arg.get();
        (this->*instruction.func)();
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

    auto left = std::move(this->stack.back());
    this->stack.pop_back();

    auto right = std::move(this->stack.back());
    this->stack.pop_back();

    auto result = f(*left.get(), *right.get());
    this->stack.push_back(operand_uptr(result));
}

// TODO: move these to an instructions cpp file
void AVM::push() {
    this->stack.push_back(operand_uptr(instr_arg->clone()));
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
    throw AVMException(Runtime, "implement this");
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
    auto item = this->stack.back().get();
    if (item->getType() != Int8)
        throw AVMException(Runtime, "print on non int8 variable");
    // TODO: figure out a way to directly use the type of an Int8 instead of respecifying it
    auto value = std::get<int8_t>(item->getValue());
    std::cout << static_cast<char>(value);
}

void AVM::exit() {
    this->exit_flag = true;
}