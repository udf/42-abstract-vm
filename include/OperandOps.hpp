#pragma once

#include <cmath>

namespace Operand {

template<typename T = void>
struct modulus;

template<>
struct modulus<void> {
    template<class T>
    T operator()(const T &x, const T &y) const {
        return x % y;
    }

    float operator()(const float &x, const float &y) const {
        return fmodf(x, y);
    }

    double operator()(const double &x, const double &y) const {
        return fmod(x, y);
    }
};

} // namespace Operand