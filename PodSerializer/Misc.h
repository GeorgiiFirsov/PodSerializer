#pragma once

//
// Converts type to itself
// 
template<typename _Type>
struct Identity {
    using type = _Type;
};

template<typename _Type>
using IdenticalType = typename Identity<_Type>::type;


//
// Compares types
// 
template<typename T, typename U>
constexpr bool operator==( Identity<T>, Identity<U> )
{
    return false;
}

template<typename T>
constexpr bool operator==( Identity<T>, Identity<T> )
{
    return true;
}