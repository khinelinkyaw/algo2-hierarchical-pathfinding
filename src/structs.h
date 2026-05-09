#ifndef STRUCTS_H
#define STRUCTS_H

template<typename T>
struct vec2
{
    T x{};
    T y{};

    vec2& operator+=(vec2 const& other)
    {
        this->x += other.x;
        this->y += other.y;
        
        return *this;
    }

    vec2() = default;
    vec2(T const& _x, T const& _y);

    vec2<T> operator+(vec2<T> const& other) const;
    vec2<T> operator-(vec2<T> const& other) const;
};

template<typename T>
inline vec2<T>::vec2(T const& _x, T const& _y)
    : x{ _x }
    , y{ _y }
{
}

template<typename T>
inline vec2<T> vec2<T>::operator+(vec2<T> const& other) const
{
    return vec2<T>(x + other.x, y + other.y);
}

template<typename T>
inline vec2<T> vec2<T>::operator-(vec2<T> const& other) const
{
    return vec2<T>(x - other.x, y - other.y);
}

#endif