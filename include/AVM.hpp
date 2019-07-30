#pragma once

#include <iostream>
#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>

#include "AVMException.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"

class AVM {
  private:
    AVM() = delete;
    AVM(const AVM &other) = delete;
    AVM &operator=(const AVM &other) = delete;

    // Internal functions
    template<typename F>
    void do_binary_op(F f = F());

    // Instructions
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

    // Lexer data
    enum eTokens {
        WHITESPACE,
        IDENTIFIER,
        NUMBER,
        L_BRACKET,
        R_BRACKET,
        COMMENT_START,
        _LENGTH
    };

    static const char *sTokenNames[eTokens::_LENGTH];

    static const std::array<std::regex, eTokens::_LENGTH> rTokens; 

    struct tToken {
        eTokens type;
        size_t col_pos;
        std::string value;
    };

    // TODO: make static
    std::vector<tToken> lex_line(std::string line);

    // Parser data
    static const OperandFactory factory;

    using instr_fptr = decltype(&AVM::exit);
    struct InstrDef {
        instr_fptr func;
        bool needs_arg;
    };
    static const std::unordered_map<std::string, InstrDef> instr_defs;

    using operand_uptr = std::unique_ptr<IOperand const>;
    struct ParsedInstruction {
        struct Environment {
            operand_uptr arg;
            size_t line_number;
        };

        instr_fptr func;
        Environment env;
    };

    std::vector<ParsedInstruction> instructions;

    // Runtime data
    ParsedInstruction::Environment const *instr_env = nullptr;
    std::vector<operand_uptr> stack;
    bool exit_flag = false;

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
