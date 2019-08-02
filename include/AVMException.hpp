#pragma once
#include <exception>
#include <string>

enum eAVMException {
    Lexer,
    Parser,
    Runtime,
    Internal,
    eAVMException_MAX
};

class AVMException : public std::exception {
  public:
    constexpr static const char *const TypeName[eAVMException_MAX] = {
      "Lex",
      "Parse",
      "Runtime",
      "Internal"
    };

    AVMException(eAVMException type, std::string info);
    AVMException(std::string info);
    AVMException(const AVMException &other);
    ~AVMException();

    auto set_type(eAVMException type) -> AVMException &;
    auto set_info(std::string info) -> AVMException &;
    auto set_line(size_t line) -> AVMException &;
    auto set_column(size_t column) -> AVMException &;
    auto set_hint(std::string hint) -> AVMException &;

    auto what() const throw() -> const char *;

  private:
    AVMException();
    auto operator=(const AVMException &other) -> AVMException &;

    auto build_pretty_info() -> void;
    template<typename T>
    auto set_member(T AVMException::* member, T value) -> AVMException &;

    std::string pretty_info;

    eAVMException type;
    std::string info;
    size_t line = 0;
    size_t column = 0;
    std::string hint;
};
