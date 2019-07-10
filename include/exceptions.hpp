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
    enum Type { Lexer, Parser, Runtime, Internal, _Length };
    constexpr static const char *const Name[Type::_Length] = {
      "Lexer",
      "Parser",
      "Runtime",
      "Internal"
    };

    AVMException(
        AVMException::Type type,
        const std::string &info,
        size_t line = 0
    );
    ~AVMException();

    const char *what() const throw();
};
