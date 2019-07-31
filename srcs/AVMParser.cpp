#include "AVM.hpp"

auto AVM::envbuilder_single(std::vector<tToken> &tokens) -> ParsedInstruction {
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

    // TODO: move this to a func
    auto &identifier = tokens[0].value;
    auto it = mapping.find(identifier);
    if (it == mapping.end())
        throw AVMException(Parser, "Unknown instruction:")
            .set_hint(identifier)
            .set_column(tokens[0].col_pos);
    p.func = (*it).second;

    return p;
}

auto AVM::envbuilder_val_arg(std::vector<tToken> &tokens) -> ParsedInstruction {
    static instr_mapping mapping{
        {"push", &AVM::push},
        {"assert", &AVM::assert},
    };
    static auto factory = OperandFactory();

    ParsedInstruction p{};

    auto &identifier = tokens[0].value;
    auto it = mapping.find(identifier);
    if (it == mapping.end())
        throw AVMException(Parser, "Unknown instruction:")
            .set_hint(identifier)
            .set_column(tokens[0].col_pos);
    p.func = (*it).second;

    try {
        p.env.arg = operand_uptr(factory.createOperand(
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
    // TODO: find a better way to do this mapping
    // a vector of struct references? (instances inside the envbuilder struct)
    static const std::vector<
        std::pair<
            envbuilder_fptr,
            std::vector<eTokens>
        >
    > patterns = {
        {
            &AVM::envbuilder_single,
            {IDENTIFIER}
        },
        {
            &AVM::envbuilder_val_arg,
            {IDENTIFIER, IDENTIFIER, L_BRACKET, NUMBER, R_BRACKET}
        }
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
    for (auto &pattern : patterns) {
        size_t i = first_diff(
            pattern.second,
            tokens,
            [](eTokens &expected_type, tToken &token) {
                return token.type == expected_type;
            }
        );
        // The pattern matched if no difference was found between the pattern in input
        // Note that the lengths need to be compared because an input longer than the pattern
        // can make the difference appear after the pattern
        // An end token would remove the need for the length check
        bool matched = i >= pattern.second.size() && tokens.size() == pattern.second.size();
        matches.push_back({matched, i});
        if (matched)
            match_count++;
    }

    if (match_count > 1) {
        throw AVMException(Internal, "fix me: more than one parse pattern matched");
    }

    // Call the environment builder if we got a match
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i].first) {
            return (this->*patterns[i].first)(tokens);
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
        info += sTokenNames[patterns[i].second[longest_match]];
        info += " ";
    }
    info += "after";

    tToken &last_token = tokens[longest_match];
    throw AVMException(Parser, info)
        .set_hint(last_token.value)
        .set_column(last_token.col_pos + last_token.value.size())
        .set_line(1); // TODO: dont set line
}
