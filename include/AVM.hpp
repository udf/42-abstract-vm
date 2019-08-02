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
    auto operator=(const AVM &other) -> AVM & = delete;


    // Internal functions
    template<typename F>
    auto do_binary_op(F f = F()) -> void;


    // Instructions
    auto _assert(bool condition, std::string info) -> void;
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
    auto exit() -> void;


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

    static auto lex_line(std::string line) -> const std::vector<tToken>;


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
        static auto get_func(instr_mapping &m, const tToken &token) -> instr_fptr;

        static auto parse_single(const std::vector<tToken> &tokens) -> ParsedInstruction;
        static auto parse_val_arg(const std::vector<tToken> &tokens) -> ParsedInstruction;

        using fptr = decltype(&InstrBuilders::parse_single);
        struct FuncData {
            fptr func;
            std::vector<eTokens> pattern;
        };

        static const FuncData single;
        static const FuncData val_arg;
    };

    static auto parse_line(std::string &line) -> std::optional<ParsedInstruction>;


    // Runtime
    std::vector<ParsedInstruction> instructions;
    size_t instruction_ptr = 0;

    IOperand const *instr_arg = nullptr;
    std::vector<operand_uptr> stack;
    bool running = true;

  public:
    AVM();
    ~AVM();

    auto load_line(std::string &line, size_t line_number = 0) -> void;
    auto step() -> void;
    auto run() -> void;
};
