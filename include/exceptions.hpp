#pragma once
#include <exception>
#include <string>

namespace Exception {
constexpr const char LEXER[] = "Lexer";
constexpr const char PARSER[] = "Parse";
constexpr const char RUNTIME[] = "Runtime";
} // namespace Exception

template<const char *type>
class AVMException : public std::exception {
  private:
    AVMException();
    AVMException(const AVMException &other);
    AVMException &operator=(const AVMException &other);

    std::string info;

  public:
    AVMException(const std::string &info) {
        this->info = std::string(type) + " error: " + info;
    }

    AVMException(int line, const std::string &info) {
        this->info = std::string(type) + " error";
        this->info += " (line " + std::to_string(line) + "): " + info;
    }

    ~AVMException() {
    }

    const char *what() const throw() {
        return this->info.c_str();
    }
};
