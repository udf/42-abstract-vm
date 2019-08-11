#pragma once

#include <iostream>
#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>
#include <list>

#include "util.hpp"
#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "AVMLexer.hpp"
#include "AVMException.hpp"

namespace AVM {

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
    auto rot() -> void;
    auto save() -> void;
    auto load() -> void;
    auto exit() -> void;

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
        static auto get_func(instr_mapping &m, const Lexer::tToken &token)
            -> instr_fptr;

        static auto parse_single(const std::vector<Lexer::tToken> &tokens)
            -> ParsedInstruction;
        static auto parse_val_arg(const std::vector<Lexer::tToken> &tokens)
            -> ParsedInstruction;

        using fptr = decltype(&InstrBuilders::parse_single);
        struct BuilderData {
            fptr func;
            std::vector<Lexer::eTokens> pattern;
        };

        static const BuilderData single;
        static const BuilderData val_arg;
    };

    static auto parse_line(std::string &line)
        -> std::optional<ParsedInstruction>;


    // Runtime
    std::vector<ParsedInstruction> instructions;
    size_t instruction_ptr = 0;
    operand_uptr stored_val;

    IOperand const *instr_arg = nullptr;
    std::list<operand_uptr> stack;
    bool running = true;

  public:
    AVM();
    ~AVM();

    auto load_line(std::string &line, size_t line_number = 0) -> void;
    auto step() -> void;
    auto run() -> void;
};

} // namespace AVM