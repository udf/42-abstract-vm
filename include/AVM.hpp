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


    // Lexer
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

    static std::vector<tToken> lex_line(std::string line);


    // Parser
    using instr_fptr = decltype(&AVM::exit);
    using instr_mapping = const std::unordered_map<std::string, instr_fptr>;

    using operand_uptr = std::unique_ptr<IOperand const>;
    // TODO: remove environment
    struct ParsedInstruction {
        struct Environment {
            operand_uptr arg;
            size_t line_number;
        };

        instr_fptr func;
        Environment env;
    };

    // TODO: move into struct
    ParsedInstruction envbuilder_single(std::vector<tToken> &tokens);
    ParsedInstruction envbuilder_val_arg(std::vector<tToken> &tokens);
    using envbuilder_fptr = decltype(&AVM::envbuilder_single);

    static ParsedInstruction parse_line(std::string &line);


    // Runtime
    std::vector<ParsedInstruction> instructions;

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
