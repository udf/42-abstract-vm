#include <memory>
#include <vector>

#include "IOperand.hpp"

struct AVMState {
    std::vector<std::unique_ptr<IOperand *>> stack;
};

// instruction value_type(value)
struct Line {
    size_t line_number;
    std::string instruction;
    std::string value_type;
    std::string value;
};