#include "AVM.hpp"

const AVM::InstrBuilders::FuncData AVM::InstrBuilders::single = {
    &InstrBuilders::parse_single,
    {IDENTIFIER}
};

const AVM::InstrBuilders::FuncData AVM::InstrBuilders::val_arg = {
    &InstrBuilders::parse_val_arg,
    {IDENTIFIER, IDENTIFIER, L_BRACKET, NUMBER, R_BRACKET}
};

AVM::instr_fptr AVM::InstrBuilders::get_func(instr_mapping &m, tToken &token) {
    auto it = m.find(token.value);
    if (it == m.end())
        throw AVMException(Parser, "Unknown instruction:")
            .set_hint(token.value)
            .set_column(token.col_pos);
    return (*it).second;
}

auto AVM::InstrBuilders::parse_single(std::vector<tToken> &tokens) -> ParsedInstruction {
    static instr_mapping mapping{
        {"pop", &AVM::pop},
        {"dump", &AVM::dump},
        {"add", &AVM::add},
        {"sub", &AVM::sub},
        {"mul", &AVM::mul},
        {"div", &AVM::div},
        {"mod", &AVM::mod},
        {"print", &AVM::print},
        {"exit", &AVM::exit},
    };

    ParsedInstruction p{};

    p.func = get_func(mapping, tokens[0]);

    return p;
}

auto AVM::InstrBuilders::parse_val_arg(std::vector<tToken> &tokens) -> ParsedInstruction {
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

// TODO: move to util file
template<typename T, typename U, typename F>
size_t first_diff(std::vector<T> s1, std::vector<U> s2, F func) {
    const size_t min_len = std::min(s1.size(), s2.size());
    for (size_t i = 0; i < min_len; i++) {
        if (!func(s1[i], s2[i]))
            return i;
    }
    return min_len;
}

auto AVM::parse_line(std::string &line) -> ParsedInstruction {
    static const std::vector<const InstrBuilders::FuncData *> builders = {
        &InstrBuilders::single,
        &InstrBuilders::val_arg
    };

    auto tokens = AVM::lex_line(line);

    // TODO: debug flag
    std::cout << "Input: " << line << std::endl;
    for (auto &token : tokens) {
        std::cout << "<";
        std::cout << sTokenNames[token.type] << "@" << token.col_pos << ", ";
        std::cout << token.value;
        std::cout << ">" << std::endl;
    }

    std::vector<std::pair<bool, size_t>> matches;
    size_t match_count = 0;
    for (auto builder : builders) {
        size_t i = first_diff(
            builder->pattern,
            tokens,
            [](eTokens &expected_type, tToken &token) {
                return token.type == expected_type;
            }
        );
        // The pattern matched if no difference was found between the pattern in input
        // Note that the lengths need to be compared because an input longer than the pattern
        // can make the difference appear after the pattern
        // An end token would remove the need for the length check
        bool matched = i >= builder->pattern.size() && tokens.size() == builder->pattern.size();
        matches.push_back({matched, i});
        if (matched)
            match_count++;
    }

    if (match_count > 1) {
        throw AVMException(Internal, "fix me: more than one builder pattern matched");
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

    // TODO: collect into set to avoid dupes
    std::string info = "Expected ";
    for (size_t i = 0; i < matches.size(); i++) {
        auto& [matched, match_pos] = matches[i];
        if (match_pos != longest_match)
            continue;
        info += sTokenNames[builders[i]->pattern[longest_match]];
        info += " ";
    }
    info += "after";

    tToken &last_token = tokens[longest_match];
    throw AVMException(Parser, info)
        .set_hint(last_token.value)
        .set_column(last_token.col_pos + last_token.value.size())
        .set_line(1); // TODO: dont set line
}
