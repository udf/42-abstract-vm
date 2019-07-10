#include "exceptions.hpp"

AVMException::AVMException(
    AVMException::Type type,
    const std::string &info,
    size_t line
) {
    this->info = info;
    this->type = type;
    this->line = line;
    this->build_pretty_info();
}

AVMException::~AVMException() {
}

const char *AVMException::what() const throw() {
    return this->pretty_info.c_str();
}

void AVMException::change_type(AVMException::Type new_type) {
    if (this->type == new_type)
        return;
    this->type = new_type;
    this->build_pretty_info();
}

void AVMException::build_pretty_info() {
    this->pretty_info = std::string(AVMException::Name[type]) + " error";
    if (line > 0)
        this->pretty_info += " (line " + std::to_string(line) + ")";
    this->pretty_info += ": " + info;
}