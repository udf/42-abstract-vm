#pragma once
#include <vector>
#include <list>

template<typename T, typename U, typename F>
auto first_diff(std::vector<T> s1, std::vector<U> s2, F func) -> size_t {
    const size_t min_len = std::min(s1.size(), s2.size());
    for (size_t i = 0; i < min_len; i++) {
        if (!func(s1[i], s2[i]))
            return i;
    }
    return min_len;
}

template<typename T>
void list_rotate(
    std::list<T> &list,
    size_t count,
    bool from_front
) {
    count = count % list.size();
    if (count == 0)
        return;
    size_t alt_count = list.size() - count;
    if (alt_count < count) {
        count = alt_count;
        from_front = !from_front;
    }

    using diff_type = typename std::list<T>::difference_type;

    if (from_front) {
        auto first = list.begin();
        auto last = std::next(list.begin(), static_cast<diff_type>(count));
        list.splice(list.end(), list, first, last);
    } else {
        auto first = std::prev(list.end(), static_cast<diff_type>(count));
        auto last = list.end();
        list.splice(list.begin(), list, first, last);
    }
}