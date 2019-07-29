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

    while (line.length() > 0) {
        std::vector<tToken> matched_tokens;

        for (size_t i = 0; i < rTokens.size(); ++i) {
            std::smatch matches;
            if (!std::regex_search(line, matches, rTokens[i]))
                continue;

            if (matches.position() != 0)
                throw AVMException(
                    "Regex match does not start at the beginning of input"
                );

            matched_tokens.push_back({
                static_cast<eTokens>(i),
                line.substr(0, static_cast<size_t>(matches[0].length()))
            });
        }

        // TODO: count removed characters and throw character position
        // also put char position in tToken so parser has that info
        if (matched_tokens.size() == 0) {
            throw AVMException(Lexer, "Unknown token");
        }
        if (matched_tokens.size() > 1) {
            for (auto &&t : matched_tokens) {
                std::cout << sTokenNames[t.type] << std::endl;
            }
            throw AVMException(Lexer, "Ambiguous token");
        }

        tToken &token = matched_tokens[0];

        if (token.type == COMMENT_START) {
            break;
        }

        if (token.type != WHITESPACE) {
            tokens.push_back(token);
        }

        line.erase(0, token.value.length());
    }

    return tokens;
}