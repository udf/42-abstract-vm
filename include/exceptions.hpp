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
    enum Type { Lexer, Parser, Runtime, _Length };
    constexpr static const char *const Name[Type::_Length] = {
        "Lexer", "Parser", "Runtime"};

    AVMException(AVMException::Type type, const std::string &info,
                 int line = -1);
    ~AVMException();

    const char *what() const throw();
};
