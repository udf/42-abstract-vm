#pragma once
#include <exception>
#include <string>

namespace AVM {

enum eException {
    Lex,
    Parse,
    Runtime,
    Internal,
    eException_MAX
};

class Exception : public std::exception {
  public:
    constexpr static const char *const TypeName[eException_MAX] = {
      "Lex",
      "Parse",
      "Runtime",
      "Internal"
    };

    Exception(eException type, std::string info);
    Exception(std::string info);
    Exception(const Exception &other);
    ~Exception();

    auto set_type(eException type) -> Exception &;
    auto set_info(std::string info) -> Exception &;
    auto set_line(size_t line) -> Exception &;
    auto set_column(size_t column) -> Exception &;
    auto set_hint(std::string hint) -> Exception &;

    auto what() const throw() -> const char *;

  private:
    Exception();
    auto operator=(const Exception &other) -> Exception &;

    auto build_pretty_info() -> void;
    template<typename T>
    auto set_member(T Exception::* member, T value) -> Exception &;

    std::string pretty_info;

    eException type;
    std::string info;
    size_t line = 0;
    size_t column = 0;
    std::string hint;
};

} // namespace AVM