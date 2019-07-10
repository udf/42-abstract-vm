#pragma once
#include <exception>
#include <string>

class AVMException : public std::exception {
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

  private:
    AVMException();
    AVMException(const AVMException &other);
    AVMException &operator=(const AVMException &other);

    void change_type(AVMException::Type new_type);
    void build_pretty_info();

    std::string info;
    std::string pretty_info;
    AVMException::Type type;
    size_t line;
};
