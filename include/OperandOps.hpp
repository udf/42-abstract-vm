#pragma once

#include <cmath>

namespace Operand {
template<class T, class U>
struct plus {
    U operator()(const T &x, const T &y) const {
        return x + y;
    }
};

template<class T, class U>
struct minus {
    U operator()(const T &x, const T &y) const {
        return x - y;
    }
};

template<class T, class U>
struct multiply {
    U operator()(const T &x, const T &y) const {
        return x * y;
    }
};

template<class T, class U>
struct divide {
    U operator()(const T &x, const T &y) const {
        return x / y;
    }
};

template<class T, class U>
struct modulus {
    U operator()(const T &x, const T &y) const {
        return x % y;
    }
};

template<class U>
struct modulus<float, U> {
    U operator()(const float &x, const float &y) const {
        return fmodf(x, y);
    }
};

template<class U>
struct modulus<double, U> {
    U operator()(const double &x, const double &y) const {
        return fmod(x, y);
    }
};
} // namespace Operand