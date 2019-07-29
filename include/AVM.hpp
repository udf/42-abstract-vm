#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "AVMException.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"

class AVM {
  private:
    AVM() = delete;
    AVM(const AVM &other) = delete;
    AVM &operator=(const AVM &other) = delete;

    template<typename F>
    void do_binary_op(F f = F());

    void push();
    void pop();
    void dump();
    void assert();
    void add();
    void sub();
    void mul();
    void div();
    void mod();
    void print();
    void exit();

    using instr_fptr = decltype(&AVM::exit);
    using operand_uptr = std::unique_ptr<IOperand const>;

    struct InstrDef {
        instr_fptr func;
        bool needs_arg;
    };

    struct ParsedInstruction {
        struct Environment {
            operand_uptr arg;
            size_t line_number;
        };

        instr_fptr func;
        Environment env;
    };

    static const std::unordered_map<std::string, InstrDef> instr_defs;
    static const OperandFactory factory;

    std::vector<operand_uptr> stack;
    bool exit_flag = false;

    ParsedInstruction::Environment const *instr_env = nullptr;
    std::vector<ParsedInstruction> instructions;

  public:

    // instruction value_type(value)
    struct Line {
        size_t line_number;
        std::string instruction;
        // TODO: put these in an optional
        std::string value_type;
        std::string value;
    };

    AVM(std::vector<Line> &lines);
    ~AVM();

    void run();
};
