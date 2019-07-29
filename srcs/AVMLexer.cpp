#include "AVM.hpp"

const char *AVM::sTokenNames[eTokens::_LENGTH] = {
    "WHITESPACE",
    "IDENTIFIER",
    "NUMBER",
    "L_BRACKET",
    "R_BRACKET",
    "COMMENT_START",
};

const std::array<std::regex, AVM::eTokens::_LENGTH> AVM::rTokens = {
    std::regex(R"(^\s+)"),
    std::regex(R"(^[a-zA-Z]\w*)"),
    std::regex(R"(^-?\d+\.?)"),
    std::regex(R"(^\()"),
    std::regex(R"(^\))"),
    std::regex(R"(^;)"),
};

auto AVM::lex_line(std::string line) -> std::vector<tToken> {
    std::vector<tToken> tokens;
    size_t col_pos = 1;

    while (line.length() > 0) {
        std::vector<tToken> matched_tokens;

        for (size_t i = 0; i < rTokens.size(); ++i) {
            std::smatch matches;
            if (!std::regex_search(line, matches, rTokens[i]))
                continue;

            if (matches.position() != 0) {
                throw AVMException(
                    "Regex match does not start at the beginning of input"
                ).set_column(col_pos + 1);
            }

            matched_tokens.emplace_back();
            tToken &token = matched_tokens.back();

            token.col_pos = col_pos;
            token.type = static_cast<eTokens>(i);
            token.value = line.substr(0, static_cast<size_t>(matches[0].length()));
        }

        if (matched_tokens.size() == 0) {
            throw AVMException(Lexer, "Unknown token").set_column(col_pos);
        }
        if (matched_tokens.size() > 1) {
            for (auto &&t : matched_tokens) {
                std::cout << sTokenNames[t.type] << std::endl;
            }
            throw AVMException(Lexer, "Ambiguous token").set_column(col_pos);
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

    return tokens;
}