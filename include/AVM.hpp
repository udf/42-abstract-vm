#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "exceptions.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"

class AVM {
  private:
    AVM() = delete;
    AVM(const AVM &other) = delete;
    AVM &operator=(const AVM &other) = delete;

    using operand_uptr = std::unique_ptr<IOperand const>;

    static const OperandFactory factory;

    std::vector<operand_uptr> stack;
    bool exit_flag = false;

    // TODO: move this definition to cpp
    template<typename F>
    void do_binary_op(F f = F()) {
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

    struct InstrDef {
        instr_fptr func;
        bool needs_arg;
    };

    static const std::unordered_map<std::string, InstrDef> instr_defs;

    IOperand const *instr_arg = nullptr;

    struct ParsedInstruction {
        instr_fptr func;
        operand_uptr arg;
    };

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
