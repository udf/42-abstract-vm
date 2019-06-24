#pragma once
#include <exception>
#include <string>

class AVMException : public std::exception {
  private:
    AVMException();
    AVMException(const AVMException &other);
    AVMException &operator=(const AVMException &other);

    std::string info;

  public:
    enum Type { LEXER, PARSER, RUNTIME, _LENGTH };
    constexpr static const char *const Name[Type::_LENGTH] = {
        "Lexer", "Parser", "Runtime"};

    AVMException(AVMException::Type type, const std::string &info,
                 int line = -1) {
        std::string type_name = AVMException::Name[type];
        this->info = type_name + " error";
        if (line > 0)
            this->info += " (line " + std::to_string(line) + ")";
        this->info += ": " + info;
    }

    ~AVMException() {
    }

    const char *what() const throw() {
        return this->info.c_str();
    }
};
