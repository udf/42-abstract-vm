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
AVMException &AVMException::set_member(T AVMException::* member, T value) {
    this->*member = value;
    this->build_pretty_info();
    return *this;
}

AVMException &AVMException::set_type(eAVMException type) {
    return set_member(&AVMException::type, type);
}

AVMException &AVMException::set_info(std::string info) {
    return set_member(&AVMException::info, info);
}

AVMException &AVMException::set_line(size_t line) {
    return set_member(&AVMException::line, line);
}

AVMException &AVMException::set_column(size_t column) {
    return set_member(&AVMException::column, column);
}

AVMException &AVMException::set_hint(std::string hint) {
    return set_member(&AVMException::hint, hint);
}

const char *AVMException::what() const throw() {
    return this->pretty_info.c_str();
}

// {type} error [on line {line}[, column {column}]]: info "{hint}"
void AVMException::build_pretty_info() {
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