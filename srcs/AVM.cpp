#include "AVM.hpp"

AVM::AVM(std::vector<Line> &lines) {
    for (auto &&line : lines) {
        std::cout << line.line_number << ": " << line.instruction << " "
                  << line.value_type << "(" << line.value << ")" << std::endl;
    }
}

AVM::~AVM() {
}