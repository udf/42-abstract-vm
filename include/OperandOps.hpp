#pragma once

#include <cmath>
#include "AVMException.hpp"

namespace Operand {

template<typename T = void>
struct divides;

template<>
struct divides<void> {
    template<class T>
    auto operator()(const T &x, const T &y) const -> T {
        if (std::equal_to()(y, static_cast<T>(0)))
            throw AVMException("Divide by zero");
        return x / y;
    }
};

template<typename T = void>
struct modulus;

template<>
struct modulus<void> {
    template<class T>
    auto operator()(const T &x, const T &y) const -> T {
        if (std::equal_to()(y, static_cast<T>(0)))
            throw AVMException("Modulus by zero");
        return x % y;
    }

    // TODO: test float division with -0
    auto operator()(const float &x, const float &y) const -> float {
        if (std::equal_to()(y, 0))
            throw AVMException("Modulus by zero");
        return fmodf(x, y);
    }

    auto operator()(const double &x, const double &y) const -> double {
        if (std::equal_to()(y, 0))
            throw AVMException("Modulus by zero");
        return fmod(x, y);
    }
};

} // namespace Operand