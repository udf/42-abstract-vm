#pragma once

#include <iostream>
#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>

#include "util.hpp"
#include "AVMException.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"

class AVM {
  private:
    AVM(const AVM &other) = delete;
    AVM &operator=(const AVM &other) = delete;


    // Internal functions
    template<typename F>
    void do_binary_op(F f = F());


    // Instructions
    void _assert(bool condition, std::string info);
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
        END,
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
    struct ParsedInstruction {
        size_t line_number;
        instr_fptr func;
        operand_uptr arg;
    };

    struct InstrBuilders {
        static instr_fptr get_func(instr_mapping &m, tToken &token);

        static ParsedInstruction parse_single(std::vector<tToken> &tokens);
        static ParsedInstruction parse_val_arg(std::vector<tToken> &tokens);

        using fptr = decltype(&InstrBuilders::parse_single);
        struct FuncData {
            fptr func;
            std::vector<eTokens> pattern;
        };

        static const FuncData single;
        static const FuncData val_arg;
    };

    static std::optional<ParsedInstruction> parse_line(std::string &line);


    // Runtime
    std::vector<ParsedInstruction> instructions;
    size_t instruction_ptr = 0;

    IOperand const *instr_arg = nullptr;
    std::vector<operand_uptr> stack;
    bool running = true;

  public:
    AVM();
    ~AVM();

    void load_line(std::string &line, size_t line_number = 0);
    void step();
    void run();
};
