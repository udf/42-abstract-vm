#include "AVM.hpp"

AVM::AVM() {
}

AVM::~AVM() {
}

auto AVM::load_line(std::string &line, size_t line_number) -> void {
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

auto AVM::step() -> void {
    if (this->instruction_ptr >= this->instructions.size())
        throw AVMException(
            Runtime,
            "Unexpected end of instructions (missing exit?)"
        );

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

auto AVM::run() -> void{
    while (this->running) {
        this->step();
    }
}

auto AVM::_assert(bool condition, std::string info) -> void {
    if (!condition) {
        throw AVMException(Runtime, info);
    }
}

template<typename F>
auto AVM::do_binary_op(F f) -> void {
    _assert(
        this->stack.size() >= 2,
        "binary operation with less than two values on the stack"
    );

    auto it = this->stack.rbegin();
    auto &right = *it;
    it++;
    auto &left = *it;

    try {
        auto result = f(*left, *right);
        this->stack.pop_back();
        this->stack.pop_back();
        this->stack.emplace_back(result);
    } catch (AVMException &e) {
        e.set_type(Runtime);
        throw;
    }
}

auto AVM::push() -> void {
    this->stack.emplace_back(instr_arg->clone());
}

auto AVM::pop() -> void {
    _assert(!this->stack.empty(), "pop on empty stack");
    this->stack.pop_back();
}

auto AVM::dump() -> void {
    size_t i = 0;

    for (auto it = this->stack.rbegin(); it != this->stack.rend(); ++it) {
        auto &item = **it;
        std::cout << i << ": ";
        std::cout << item.toPrettyString();
        std::cout << std::endl;
        i++;
    }
}

auto AVM::assert() -> void {
    if (*instr_arg == *stack.back())
        return;
    std::string info = "Assertion error, expected \"";
    info += instr_arg->toPrettyString() + "\" found";
    throw AVMException(Runtime, info)
        .set_hint(stack.back().get()->toPrettyString());
}

auto AVM::add() -> void {
    this->do_binary_op<std::plus<>>();
}

auto AVM::sub() -> void {
    this->do_binary_op<std::minus<>>();
}

auto AVM::mul() -> void {
    this->do_binary_op<std::multiplies<>>();
}

auto AVM::div() -> void {
    this->do_binary_op<std::divides<>>();
}

auto AVM::mod() -> void {
    this->do_binary_op<std::modulus<>>();
}

auto AVM::print() -> void {
    _assert(this->stack.size() >= 1, "print on empty stack");
    auto &item = *this->stack.back();
    _assert(item.getType() == Int8, "print on non int8 variable");
    auto value = std::get<tOperandType<Int8>::type>(item.getValue());
    std::cout << static_cast<char>(value);
}

auto AVM::rot() -> void {
    _assert(this->stack.size() >= 1, "rotate on empty stack");
    auto &item = *this->stack.back();
    _assert(item.getType() == Int32, "rotate on non int32 variable");

    auto count = std::get<tOperandType<Int32>::type>(item.getValue());
    this->stack.pop_back();
    list_rotate(
        this->stack,
        static_cast<size_t>(std::abs(count)),
        count < 0
    );
}

auto AVM::save() -> void {
    _assert(this->stack.size() >= 1, "save on empty stack");
    this->stored_val.reset(this->stack.back()->clone());
}

auto AVM::load() -> void {
    _assert((bool)this->stored_val, "load when no value is saved");
    this->stack.emplace_back(this->stored_val->clone());
}

auto AVM::exit() -> void {
    this->running = false;
}