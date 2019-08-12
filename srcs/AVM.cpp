#include "AVM.hpp"
#include "AVMParser.hpp"

namespace AVM {

const AVM::instr_mapping AVM::instr_map{
    {"push", &AVM::push},
    {"pop", &AVM::pop},
    {"dump", &AVM::dump},
    {"assert", &AVM::assert},
    {"add", &AVM::add},
    {"sub", &AVM::sub},
    {"mul", &AVM::mul},
    {"div", &AVM::div},
    {"mod", &AVM::mod},
    {"print", &AVM::print},
    {"rot", &AVM::rot},
    {"save", &AVM::save},
    {"load", &AVM::load},
    {"exit", &AVM::exit},
    {"jmp", &AVM::jmp},
    {"je", &AVM::je},
    {"jne", &AVM::jne},
    {"jlt", &AVM::jlt},
    {"jgt", &AVM::jgt},
    {"jlte", &AVM::jlte},
    {"jgte", &AVM::jgte},
    {"call", &AVM::call},
    {"ret", &AVM::ret},
};

AVM::AVM() {
}

AVM::~AVM() {
}

auto AVM::load_line(std::string &line, size_t line_number) -> void {
    try {
        auto result = Parser::parse_line(line);
        if (std::holds_alternative<std::monostate>(result))
            return;
        if (std::holds_alternative<Instruction>(result)) {
            auto &instruction = std::get<Instruction>(result);
            this->instructions.push_back(std::move(instruction));
            this->instructions.back().line_number = line_number;
        }
        if (std::holds_alternative<Parser::Label>(result)) {
            auto &label = std::get<Parser::Label>(result);
            if (this->labels.count(label.name) > 0)
                throw Exception(Parse, "Duplicate label")
                    .set_hint(label.name);
            this->labels[label.name] = this->instructions.size();
        }
    } catch (Exception &e) {
        e.set_line(line_number);
        throw;
    }
}

auto AVM::step() -> void {
    if (this->instruction_ptr >= this->instructions.size())
        throw Exception(
            Runtime,
            "Unexpected end of instructions (missing exit?)"
        );

    auto &instruction = this->instructions.at(this->instruction_ptr);

    this->instruction_ptr++;
    this->cur_instr = &instruction;
    try {
        (this->*instruction.func)();
    } catch (Exception &e) {
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
        throw Exception(Runtime, info);
    }
}

template<typename T, typename F>
auto AVM::do_binary_op(F f) -> T {
    _assert(
        this->stack.size() >= 2,
        "binary operation with less than two values on the stack"
    );

    auto it = this->stack.rbegin();
    auto &right = *it;
    it++;
    auto &left = *it;

    return f(*left, *right);
}

template<typename F>
auto AVM::do_math_op(F f) -> void {
    try {
        auto result = do_binary_op<operand_uptr::pointer>(f);
        this->stack.pop_back();
        this->stack.pop_back();
        this->stack.emplace_back(result);
    } catch (Exception &e) {
        e.set_type(Runtime);
        throw;
    }
}

template<typename F>
auto AVM::do_jump_op(F f) -> void {
    auto result = do_binary_op<bool>(f);
    this->stack.pop_back();
    if (result)
        this->jmp();
}


auto AVM::push() -> void {
    this->stack.emplace_back(cur_instr->arg->clone());
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
    _assert(!this->stack.empty(), "assert on empty stack");
    if (cur_instr->arg->strict_equals(*stack.back()))
        return;
    std::string info = "Assertion error, expected \"";
    info += cur_instr->arg->toPrettyString() + "\" found";
    throw Exception(Runtime, info)
        .set_hint(stack.back()->toPrettyString());
}

auto AVM::add() -> void {
    this->do_math_op<std::plus<>>();
}

auto AVM::sub() -> void {
    this->do_math_op<std::minus<>>();
}

auto AVM::mul() -> void {
    this->do_math_op<std::multiplies<>>();
}

auto AVM::div() -> void {
    this->do_math_op<std::divides<>>();
}

auto AVM::mod() -> void {
    this->do_math_op<std::modulus<>>();
}

auto AVM::print() -> void {
    _assert(this->stack.size() >= 1, "print on empty stack");
    auto &item = *this->stack.back();
    _assert(item.getType() == Int8, "print on non int8 variable");
    auto value = std::get<tOperandType<Int8>::type>(item.getValue());
    std::cout << static_cast<char>(value);
}

auto AVM::exit() -> void {
    this->running = false;
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
        count > 0
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


auto AVM::jmp() -> void {
    try {
        this->instruction_ptr = this->labels.at(cur_instr->str_arg);
    } catch (std::out_of_range) {
        throw Exception(Runtime, "Jump to unknown label")
            .set_hint(cur_instr->str_arg);
    }
};

auto AVM::je() -> void {
    this->do_jump_op<std::equal_to<>>();
}

auto AVM::jne() -> void {
    this->do_jump_op<std::not_equal_to<>>();
}

auto AVM::jlt() -> void {
    this->do_jump_op<std::less<>>();
}

auto AVM::jgt() -> void {
    this->do_jump_op<std::greater<>>();
}

auto AVM::jlte() -> void {
    this->do_jump_op<std::less_equal<>>();
}

auto AVM::jgte() -> void {
    this->do_jump_op<std::greater_equal<>>();
}


auto AVM::call() -> void {
    this->call_stack.push_back(this->instruction_ptr);
    this->jmp();
}

auto AVM::ret() -> void {
    _assert(this->call_stack.size() >= 1, "Return on empty call stack");
    this->instruction_ptr = this->call_stack.back();
    this->call_stack.pop_back();
}

} // namespace AVM