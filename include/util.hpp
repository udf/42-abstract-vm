#pragma once
#include <vector>

template<typename T, typename U, typename F>
auto first_diff(std::vector<T> s1, std::vector<U> s2, F func) -> size_t {
    const size_t min_len = std::min(s1.size(), s2.size());
    for (size_t i = 0; i < min_len; i++) {
        if (!func(s1[i], s2[i]))
            return i;
    }
    return min_len;
}
