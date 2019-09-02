#include <set>

#include "AVMParser.hpp"

namespace AVM::Parser {

const InstrBuilders::BuilderDef InstrBuilders::single = {
    &InstrBuilders::parse_single,
    {Lexer::IDENTIFIER, Lexer::END}
};

const InstrBuilders::BuilderDef InstrBuilders::val_arg = {
    &InstrBuilders::parse_val_arg,
    {
        Lexer::IDENTIFIER, Lexer::IDENTIFIER,
        Lexer::L_BRACKET, Lexer::NUMBER, Lexer::R_BRACKET,
        Lexer::END
    }
};

const InstrBuilders::BuilderDef InstrBuilders::label = {
    &InstrBuilders::parse_label,
    {Lexer::IDENTIFIER, Lexer::COLON, Lexer::END}
};

const InstrBuilders::BuilderDef InstrBuilders::id_arg = {
    &InstrBuilders::parse_id_arg,
    {Lexer::IDENTIFIER, Lexer::IDENTIFIER, Lexer::END}
};

auto InstrBuilders::get_func(const AVM::instr_mapping &m, const Lexer::tToken &token)
    -> AVM::instr_fptr
{
    auto it = m.find(token.value);
    if (it == m.end())
        throw Exception(Parse, "Unknown instruction:")
            .set_hint(token.value)
            .set_column(token.col_pos);
    return (*it).second;
}

static AVM::instr_mapping get_mapping(const std::initializer_list<std::string> names) {
    AVM::instr_mapping mapping;
    for (auto &&name : names) {
        try {
            mapping.emplace(name, AVM::instr_map.at(name));
        } catch (std::out_of_range) {
            throw Exception(Internal, "Unknown instruction in mapping definition")
                .set_hint(name);
        }
    }
    return mapping;
}

auto InstrBuilders::parse_single(const std::vector<Lexer::tToken> &tokens)
    -> parse_result
{
    static const AVM::instr_mapping mapping = get_mapping({
        "pop", "dump", "add", "sub", "mul", "div", "mod", "print", "exit",
        "rot", "save", "load", "ret", "sleep", "putchar"
    });

    AVM::Instruction p{};

    p.func = get_func(mapping, tokens[0]);

    return p;
}

auto InstrBuilders::parse_val_arg(const std::vector<Lexer::tToken> &tokens)
    -> parse_result
{
    static AVM::instr_mapping mapping = get_mapping({
        "push", "assert"
    });
    static auto factory = OperandFactory();

    AVM::Instruction p{};

    p.func = get_func(mapping, tokens[0]);

    try {
        p.arg = AVM::operand_uptr(factory.createOperand(
            tokens[1].value.c_str(),
            tokens[3].value
        ));
    } catch (Exception &e) {
        e.set_type(Parse);
        throw;
    }

    return p;
}

auto InstrBuilders::parse_label(const std::vector<Lexer::tToken> &tokens)
    -> parse_result
{
    Label label;
    label.name = tokens[0].value;
    return label;
}

auto InstrBuilders::parse_id_arg(const std::vector<Lexer::tToken> &tokens)
    -> parse_result
{
    static AVM::instr_mapping mapping = get_mapping({
        "jmp", "je", "jne", "jlt", "jgt", "jlte", "jgte", "call"
    });

    AVM::Instruction p{};

    p.func = get_func(mapping, tokens[0]);
    p.str_arg = tokens[1].value;

    return p;
}

auto parse_line(std::string &line) -> parse_result {
    static const std::vector<const InstrBuilders::BuilderDef *> builders = {
        &InstrBuilders::single,
        &InstrBuilders::val_arg,
        &InstrBuilders::label,
        &InstrBuilders::id_arg
    };

    auto tokens = Lexer::lex_line(line);
    if (tokens.size() == 1 && tokens[0].type == Lexer::END)
        return std::monostate{};

    std::vector<std::pair<bool, size_t>> matches;
    size_t match_count = 0;
    for (auto builder : builders) {
        size_t i = first_diff(
            builder->pattern,
            tokens,
            [](const Lexer::eTokens &expected_type, const Lexer::tToken &token) {
                return token.type == expected_type;
            }
        );
        bool matched = i >= builder->pattern.size();
        matches.push_back({matched, i});
        if (matched)
            match_count++;
    }

    if (match_count > 1) {
        throw Exception(
            Internal,
            "fix me: more than one builder pattern matched"
        );
    }

    // Call the environment builder if we got a match
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i].first) {
            return (builders[i]->func)(tokens);
        }
    }

    // Throw when no match was found
    size_t longest_match = 0;
    for (auto& [matched, match_pos] : matches) {
        longest_match = std::max(longest_match, match_pos);
    }

    std::set<const char *> expected_tokens;
    for (size_t i = 0; i < matches.size(); i++) {
        auto& [matched, match_pos] = matches[i];
        if (match_pos != longest_match)
            continue;
        expected_tokens.insert(
            Lexer::sTokenNames[builders[i]->pattern[longest_match]]
        );
    }

    std::string info = "Expected one of the following: ";
    for (const char *token : expected_tokens) {
        info += token;
        info += ',';
    }
    info.pop_back();
    info += longest_match == 0 ? " at" : " after";

    const Lexer::tToken &last_token = tokens[longest_match > 0 ? longest_match - 1 : 0];
    throw Exception(Parse, info)
        .set_hint(last_token.value)
        .set_column(last_token.col_pos + last_token.value.size());
}

} // namespace AVM::Parser