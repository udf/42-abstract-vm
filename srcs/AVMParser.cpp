#include "AVM.hpp"
#include <set>

namespace AVM {

const AVM::InstrBuilders::BuilderData AVM::InstrBuilders::single = {
    &InstrBuilders::parse_single,
    {Lexer::IDENTIFIER, Lexer::END}
};

const AVM::InstrBuilders::BuilderData AVM::InstrBuilders::val_arg = {
    &InstrBuilders::parse_val_arg,
    {Lexer::IDENTIFIER, Lexer::IDENTIFIER, Lexer::L_BRACKET, Lexer::NUMBER, Lexer::R_BRACKET, Lexer::END}
};

auto AVM::InstrBuilders::get_func(instr_mapping &m, const Lexer::tToken &token)
    -> instr_fptr
{
    auto it = m.find(token.value);
    if (it == m.end())
        throw AVMException(Parser, "Unknown instruction:")
            .set_hint(token.value)
            .set_column(token.col_pos);
    return (*it).second;
}

auto AVM::InstrBuilders::parse_single(const std::vector<Lexer::tToken> &tokens)
    -> ParsedInstruction
{
    static instr_mapping mapping{
        {"pop", &AVM::pop},
        {"dump", &AVM::dump},
        {"add", &AVM::add},
        {"sub", &AVM::sub},
        {"mul", &AVM::mul},
        {"div", &AVM::div},
        {"mod", &AVM::mod},
        {"print", &AVM::print},
        {"rot", &AVM::rot},
        {"save", &AVM::save},
        {"load", &AVM::load},
        {"exit", &AVM::exit},
    };

    ParsedInstruction p{};

    p.func = get_func(mapping, tokens[0]);

    return p;
}

auto AVM::InstrBuilders::parse_val_arg(const std::vector<Lexer::tToken> &tokens)
    -> ParsedInstruction
{
    static instr_mapping mapping{
        {"push", &AVM::push},
        {"assert", &AVM::assert},
    };
    static auto factory = OperandFactory();

    ParsedInstruction p{};

    p.func = get_func(mapping, tokens[0]);

    try {
        p.arg = operand_uptr(factory.createOperand(
            tokens[1].value.c_str(),
            tokens[3].value
        ));
    } catch (AVMException &e) {
        e.set_type(Parser);
        throw;
    }

    return p;
}

auto AVM::parse_line(std::string &line) -> std::optional<ParsedInstruction> {
    static const std::vector<const InstrBuilders::BuilderData *> builders = {
        &InstrBuilders::single,
        &InstrBuilders::val_arg
    };

    auto tokens = Lexer::lex_line(line);
    if (tokens.size() == 1 && tokens[0].type == Lexer::END)
        return std::nullopt;

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
        throw AVMException(
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
    info += " after";

    const Lexer::tToken &last_token = tokens[longest_match - 1];
    throw AVMException(Parser, info)
        .set_hint(last_token.value)
        .set_column(last_token.col_pos + last_token.value.size());
}

} // namespace AVM