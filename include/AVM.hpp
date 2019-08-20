#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <unordered_map>

#include "util.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "AVMException.hpp"

namespace AVM {

class AVM {
  public:
    AVM();
    ~AVM();

    auto dummy_instruction() -> void;

    auto load_line(std::string &line, size_t line_number = 0) -> void;
    auto run(bool require_exit) -> void;
    auto is_running() -> bool;

    using operand_uptr = std::unique_ptr<IOperand const>;
    using instr_fptr = decltype(&AVM::dummy_instruction);
    struct Instruction {
        size_t line_number;
        instr_fptr func;
        operand_uptr arg;
        std::string str_arg;
    };
    using instr_mapping = std::unordered_map<std::string, AVM::instr_fptr>;
    static const instr_mapping instr_map;

  private:
    AVM(const AVM &other) = delete;
    auto operator=(const AVM &other) -> AVM & = delete;


    auto step() -> bool;
    template<typename T, typename F>
    auto do_binary_op(F f = F()) -> T;
    template<typename F>
    auto do_math_op(F f = F()) -> void;
    template<typename F>
    auto do_jump_op(F f = F()) -> void;
    auto _assert(bool condition, std::string info) -> void;


    auto push() -> void;
    auto pop() -> void;
    auto dump() -> void;
    auto assert_v() -> void;
    auto add() -> void;
    auto sub() -> void;
    auto mul() -> void;
    auto div() -> void;
    auto mod() -> void;
    auto print() -> void;
    auto exit() -> void;

    auto rot() -> void;
    auto save() -> void;
    auto load() -> void;

    auto jmp() -> void;
    auto je() -> void;
    auto jne() -> void;
    auto jlt() -> void;
    auto jgt() -> void;
    auto jlte() -> void;
    auto jgte() -> void;

    auto call() -> void;
    auto ret() -> void;

    auto dbg() -> void;


    std::vector<Instruction> instructions;
    std::unordered_map<std::string, size_t> labels;
    size_t instruction_ptr = 0;
    operand_uptr stored_val;

    const Instruction *cur_instr;
    std::list<operand_uptr> stack;
    std::vector<size_t> call_stack;
    bool running = true;
};

} // namespace AVM