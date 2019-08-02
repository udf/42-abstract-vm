#include "AVMException.hpp"

AVMException::AVMException(
    eAVMException type,
    const std::string info
) {
    this->type = type;
    this->info = info;
    this->build_pretty_info();
}

AVMException::AVMException(
    const std::string info
) : AVMException(Internal, info)
{
}

AVMException::AVMException(const AVMException &other) {
    this->pretty_info = other.pretty_info;
    this->type = other.type;
    this->info = other.info;
    this->line = other.line;
    this->column = other.column;
    this->hint = other.hint;
}

AVMException::~AVMException() {
}

template<typename T>
auto AVMException::set_member(T AVMException::* member, T value)
    -> AVMException &
{
    this->*member = value;
    this->build_pretty_info();
    return *this;
}

auto AVMException::set_type(eAVMException type) -> AVMException &{
    return set_member(&AVMException::type, type);
}

auto AVMException::set_info(std::string info) -> AVMException &{
    return set_member(&AVMException::info, info);
}

auto AVMException::set_line(size_t line) -> AVMException &{
    return set_member(&AVMException::line, line);
}

auto AVMException::set_column(size_t column) -> AVMException &{
    return set_member(&AVMException::column, column);
}

auto AVMException::set_hint(std::string hint) -> AVMException &{
    return set_member(&AVMException::hint, hint);
}

auto AVMException::what() const throw() -> const char *{
    return this->pretty_info.c_str();
}

// {type} error [on line {line}[, column {column}]]: info "{hint}"
auto AVMException::build_pretty_info() -> void{
    this->pretty_info = std::string(this->TypeName[this->type]) + " error";
    if (this->line) {
        this->pretty_info += " on line " + std::to_string(this->line);
        if (this->column) {
            this->pretty_info += ", column " + std::to_string(this->column);
        }
    }
    this->pretty_info += ": " + this->info;
    if (!this->hint.empty()) {
        this->pretty_info += " \"" + this->hint + '"';
    }
}