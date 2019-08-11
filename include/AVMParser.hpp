#pragma once
#include <string>
#include <unordered_map>

#include "AVM.hpp"
#include "AVMLexer.hpp"

namespace AVM::Parser {

using instr_mapping = const std::unordered_map<std::string, AVM::instr_fptr>;

struct InstrBuilders {
    static auto get_func(instr_mapping &m, const Lexer::tToken &token)
        -> AVM::instr_fptr;

    static auto parse_single(const std::vector<Lexer::tToken> &tokens)
        -> AVM::Instruction;
    static auto parse_val_arg(const std::vector<Lexer::tToken> &tokens)
        -> AVM::Instruction;

    using fptr = decltype(&InstrBuilders::parse_single);
    struct BuilderDef {
        fptr func;
        std::vector<Lexer::eTokens> pattern;
    };

    static const BuilderDef single;
    static const BuilderDef val_arg;
};

auto parse_line(std::string &line)
    -> std::optional<AVM::Instruction>;

} // namespace AVM::Parser