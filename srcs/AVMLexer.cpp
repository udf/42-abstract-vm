#include "AVMLexer.hpp"

namespace AVM::Lexer {

const char *sTokenNames[eTokens::_LENGTH] = {
    "WHITESPACE",
    "IDENTIFIER",
    "NUMBER",
    "L_BRACKET",
    "R_BRACKET",
    "COMMENT_START",
    "END"
};

const std::array<std::regex, eTokens::_LENGTH> rTokens = {
    std::regex(R"(^\s+)"),
    std::regex(R"(^[a-zA-Z]\w*)"),
    std::regex(R"(^-?\d+\.?)"),
    std::regex(R"(^\()"),
    std::regex(R"(^\))"),
    std::regex(R"(^;)"),
    std::regex(R"(^$)"),
};

auto lex_line(std::string line) -> const std::vector<tToken> {
    std::vector<tToken> tokens;
    size_t col_pos = 1;

    while (line.length() > 0) {
        std::vector<tToken> matched_tokens;

        for (size_t i = 0; i < rTokens.size(); ++i) {
            std::smatch matches;
            if (!std::regex_search(line, matches, rTokens[i]))
                continue;

            if (matches.position() != 0) {
                throw Exception(
                    "Regex match does not start at the beginning of input"
                ).set_column(col_pos + 1);
            }

            matched_tokens.emplace_back();
            tToken &token = matched_tokens.back();

            token.col_pos = col_pos;
            token.type = static_cast<eTokens>(i);
            token.value = line.substr(
                0,
                static_cast<size_t>(matches[0].length())
            );
        }

        if (matched_tokens.size() == 0) {
            throw Exception(Lex, "Unknown token").set_column(col_pos);
        }
        if (matched_tokens.size() > 1) {
            for (auto &&t : matched_tokens) {
                std::cout << sTokenNames[t.type] << std::endl;
            }
            throw Exception(Lex, "Ambiguous token").set_column(col_pos);
        }

        tToken &token = matched_tokens[0];

        if (token.type == COMMENT_START) {
            break;
        }

        if (token.type != WHITESPACE) {
            tokens.push_back(token);
        }

        col_pos += token.value.length();
        line.erase(0, token.value.length());
    }

    tToken token;
    token.col_pos = col_pos;
    token.type = END;
    tokens.push_back(token);
    return tokens;
}

} // namespace AVM::Lexer