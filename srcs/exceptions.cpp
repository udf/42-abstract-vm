#include "exceptions.hpp"

AVMException::AVMException(
    AVMException::Type type,
    const std::string &info,
    size_t line
) {
    this->info = info;
    this->type = type;
    this->line = line;

    this->pretty_info = std::string(AVMException::Name[type]) + " error";
    if (line > 0)
        this->pretty_info += " (line " + std::to_string(line) + ")";
    this->pretty_info += ": " + info;
}

AVMException::~AVMException() {
}

const char *AVMException::what() const throw() {
    return this->pretty_info.c_str();
}