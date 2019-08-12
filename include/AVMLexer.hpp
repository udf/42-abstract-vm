#pragma once
#include <regex>
#include <array>
#include <string>
#include <iostream>

#include "AVMException.hpp"

namespace AVM::Lexer {

enum eTokens {
    WHITESPACE,
    IDENTIFIER,
    NUMBER,
    L_BRACKET,
    R_BRACKET,
    COMMENT_START,
    COLON,
    END,
    _LENGTH
};

extern const char *sTokenNames[eTokens::_LENGTH];

extern const std::array<std::regex, eTokens::_LENGTH> rTokens;

struct tToken {
    eTokens type;
    size_t col_pos;
    std::string value;
};

auto lex_line(std::string line) -> const std::vector<tToken>;

} // namespace AVM::Lexer