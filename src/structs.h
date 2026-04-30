#ifndef STRUCTS_H
#define STRUCTS_H

template<typename T>
struct vec2
{
    T x{};
    T y{};

    vec2() = default;
    vec2(T const& _x, T const& _y);
};

template<typename T>
inline vec2<T>::vec2(T const& _x, T const& _y)
    : x{ _x }
    , y{ _y }
{
};

#endif