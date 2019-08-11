#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <list>

#include "util.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "AVMException.hpp"

namespace AVM {

class AVM {
  private:
    AVM(const AVM &other) = delete;
    auto operator=(const AVM &other) -> AVM & = delete;

    template<typename F>
    auto do_binary_op(F f = F()) -> void;
    auto _assert(bool condition, std::string info) -> void;

  public:
    auto push() -> void;
    auto pop() -> void;
    auto dump() -> void;
    auto assert() -> void;
    auto add() -> void;
    auto sub() -> void;
    auto mul() -> void;
    auto div() -> void;
    auto mod() -> void;
    auto print() -> void;
    auto rot() -> void;
    auto save() -> void;
    auto load() -> void;
    auto exit() -> void;

    AVM();
    ~AVM();

    auto load_line(std::string &line, size_t line_number = 0) -> void;
    auto step() -> void;
    auto run() -> void;

    using operand_uptr = std::unique_ptr<IOperand const>;
    using instr_fptr = decltype(&AVM::exit);
    struct Instruction {
        size_t line_number;
        instr_fptr func;
        operand_uptr arg;
    };

  private:
    std::vector<Instruction> instructions;
    size_t instruction_ptr = 0;
    operand_uptr stored_val;

    IOperand const *instr_arg = nullptr;
    std::list<operand_uptr> stack;
    bool running = true;
};

} // namespace AVM