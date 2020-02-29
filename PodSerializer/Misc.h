#pragma once

//
// Converts type to itself.
// Used for construct constexpr light-weight values.
// 

template<typename _Type>
struct Identity {
    using type = _Type;
};

template<typename _Type>
using IdenticalType = typename Identity<_Type>::type;

/************************************************************************************/

//
// Metafunction to unwind one level of Identity
// 

template<
    typename _Identity /* Instantiation of Identity template */
> struct _UnwindIdentity {
    /* In order to be applied in type_list::Apply
     * we use here type alias named 'type' */
    using type = typename _Identity::type;
};

/************************************************************************************/

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

/************************************************************************************/

//
// Macro to make code more readable
// 

#define NOT_IMPLEMENTED