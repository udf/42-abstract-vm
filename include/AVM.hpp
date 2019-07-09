#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "IOperand.hpp"

class AVM {
  private:
    AVM() = delete;
    AVM(const AVM &other) = delete;
    AVM &operator=(const AVM &other) = delete;

    std::vector<std::unique_ptr<IOperand *>> stack;

  public:
    // instruction value_type(value)
    struct Line {
        size_t line_number;
        std::string instruction;
        std::string value_type;
        std::string value;
    };

    AVM(std::vector<Line> &lines);
    ~AVM();
};