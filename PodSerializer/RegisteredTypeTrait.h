#pragma once

#include "pch.h"

#include <type_traits>

#define TRAIT_REGISTER_TYPE( _Type ) \
    template<> struct is_registered_type<_Type> : std::true_type { }

template<typename _Type>
struct is_registered_type : std::false_type { };

TRAIT_REGISTER_TYPE(unsigned char);
TRAIT_REGISTER_TYPE(unsigned short);
TRAIT_REGISTER_TYPE(unsigned int);
TRAIT_REGISTER_TYPE(unsigned long);
TRAIT_REGISTER_TYPE(unsigned long long);
TRAIT_REGISTER_TYPE(signed char);
TRAIT_REGISTER_TYPE(short);
TRAIT_REGISTER_TYPE(int);
TRAIT_REGISTER_TYPE(long);
TRAIT_REGISTER_TYPE(long long);
TRAIT_REGISTER_TYPE(char);
TRAIT_REGISTER_TYPE(wchar_t);
TRAIT_REGISTER_TYPE(char16_t);
TRAIT_REGISTER_TYPE(char32_t);
TRAIT_REGISTER_TYPE(float);
TRAIT_REGISTER_TYPE(double);
TRAIT_REGISTER_TYPE(long double);
TRAIT_REGISTER_TYPE(bool);
TRAIT_REGISTER_TYPE(void*);
TRAIT_REGISTER_TYPE(const void*);
TRAIT_REGISTER_TYPE(volatile void*);
TRAIT_REGISTER_TYPE(const volatile void*);
TRAIT_REGISTER_TYPE(nullptr_t);

template<typename _Type>
using is_registered_or_aliased = \
    std::disjunction<
        is_registered_type<_Type>,
        std::is_enum<_Type>
    >;