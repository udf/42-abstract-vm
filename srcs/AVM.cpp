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
            throw AVMException(
                AVMException::Parser,
                "Unknown instruction",
                line.line_number
            );
        }

        // TODO: check if this is doing a copy
        const InstrDef &func = it->second;
        bool line_has_arg = !line.value.empty();

        if (func.needs_arg != line_has_arg) {
            throw AVMException(
                AVMException::Parser,
                func.needs_arg 
                    ? "Instruction needs an argument but none was provided"
                    : "Instruction takes no arguments but one was provided",
                line.line_number
            );
        }

        if (!func.needs_arg) {
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
    for (auto &&instruction : this->instructions) {
        this->instr_arg = instruction.arg.get();
        (this->*instruction.func)();
    }
}

// TODO: move these to an instructions cpp file
void AVM::push() {
    this->stack.push_back(operand_uptr(instr_arg->clone()));
}

void AVM::pop() {
    if (this->stack.empty())
        throw AVMException(
            AVMException::Runtime,
            "pop on empty stack",
            0 // TODO: use environment struct instead of a single arg
        );
    this->stack.pop_back();
}

void AVM::dump() {
    size_t i = 0;

    for (auto it = this->stack.rbegin(); it != this->stack.rend(); ++it) {
        auto &item = **it;
        // TODO: print type string instead of number
        std::cout << i << ": " << item.getPrecision()
            << "(" << item.toString() << ")" << std::endl;
        i++;
    }
}

void AVM::assert() {
    throw AVMException(AVMException::Runtime, "implement this");
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
        throw AVMException(AVMException::Runtime, "print on empty stack");
    auto item = this->stack.back().get();
    if (item->getType() != Int8)
        throw AVMException(AVMException::Runtime, "print on non int8 variable");
    // TODO: figure out a way to directly use the type of an Int8 instead of respecifying it
    auto value = std::get<int8_t>(item->getValue());
    std::cout << static_cast<char>(value);
}

void AVM::exit() {
    this->exit_flag = true;
}