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
      "Lexer",
      "Parser",
      "Runtime",
      "Internal"
    };

    AVMException(eAVMException type, std::string info);
    AVMException(std::string info);
    AVMException(const AVMException &other);
    ~AVMException();

    AVMException &set_type(eAVMException type);
    AVMException &set_info(std::string info);
    AVMException &set_line(size_t line);
    AVMException &set_column(size_t column);
    AVMException &set_hint(std::string hint);

    const char *what() const throw();

  private:
    AVMException();
    AVMException &operator=(const AVMException &other);

    void build_pretty_info();
    template<typename T>
    AVMException &set_member(T AVMException::* member, T value);

    std::string pretty_info;

    eAVMException type;
    std::string info;
    size_t line = 0;
    size_t column = 0;
    std::string hint;
};
