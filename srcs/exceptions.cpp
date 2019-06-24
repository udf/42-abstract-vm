#include "exceptions.hpp"

AVMException::AVMException(AVMException::Type type, const std::string &info,
                           int line) {
    std::string type_name = AVMException::Name[type];
    this->info = type_name + " error";
    if (line > 0)
        this->info += " (line " + std::to_string(line) + ")";
    this->info += ": " + info;
}

AVMException::~AVMException() {
}

const char *AVMException::what() const throw() {
    return this->info.c_str();
}