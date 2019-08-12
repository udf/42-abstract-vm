#pragma once
#include <string>
#include <unordered_map>
#include <variant>

#include "AVM.hpp"
#include "AVMLexer.hpp"

namespace AVM::Parser {

struct Label {
    std::string name;
};

using parse_result = std::variant<std::monostate, AVM::Instruction, Label>;

struct InstrBuilders {
    static auto get_func(const AVM::instr_mapping &m, const Lexer::tToken &token)
        -> AVM::instr_fptr;

    static auto parse_single(const std::vector<Lexer::tToken> &tokens)
        -> parse_result;
    static auto parse_val_arg(const std::vector<Lexer::tToken> &tokens)
        -> parse_result;
    static auto parse_label(const std::vector<Lexer::tToken> &tokens)
        -> parse_result;
    static auto parse_id_arg(const std::vector<Lexer::tToken> &tokens)
        -> parse_result;

    using fptr = decltype(&InstrBuilders::parse_single);
    struct BuilderDef {
        fptr func;
        std::vector<Lexer::eTokens> pattern;
    };

    static const BuilderDef single;
    static const BuilderDef val_arg;
    static const BuilderDef label;
    static const BuilderDef id_arg;
};

auto parse_line(std::string &line)
    -> parse_result;

} // namespace AVM::Parser