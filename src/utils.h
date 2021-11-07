// Created by chengke on 2021/11/06

#ifndef COLOR4_UTILS_H
#define COLOR4_UTILS_H

#include <stdlib.h>
#include <tuple>

#ifdef NDEBUG
#define log_debug() do {} while (0)
#else
#define log_debug(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#endif

/// Get the type of the N-th element.
template<int N, class... Args>
using get_type = typename std::tuple_element<N, std::tuple<Args...>>::type;

/// Get the value of the N-th element.
template<int N, class... Args>
constexpr decltype(auto) get(Args&&... args)
{
    return std::get<N>(std::forward_as_tuple(args...));
}

/// Get the length of Args.
template<class... Args>
constexpr auto length_of = sizeof...(Args);

constexpr auto round_up(int x, int align)
{
    return (x + align - 1) / align * align;
}

inline bool ispower2(size_t x) 
{
    return (x & -x) == x && x > 0;
}

inline size_t upperpower2(size_t x) 
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
    return x;
}

#endif // COLOR4_UTILS_H
