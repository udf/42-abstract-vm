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

// TODO: templated set member?
AVMException &AVMException::set_type(eAVMException type) {
    this->type = type;
    this->build_pretty_info();
    return *this;
}

AVMException &AVMException::set_info(std::string info) {
    this->info = info;
    this->build_pretty_info();
    return *this;
}

AVMException &AVMException::set_line(size_t line) {
    this->line = line;
    this->build_pretty_info();
    return *this;
}

AVMException &AVMException::set_column(size_t column) {
    this->column = column;
    this->build_pretty_info();
    return *this;
}

AVMException &AVMException::set_hint(std::string hint) {
    this->hint = hint;
    this->build_pretty_info();
    return *this;
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