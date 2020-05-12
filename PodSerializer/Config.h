#pragma once

//
// Minimal requirements checking
// 
#if defined(_MSC_VER)
#   if _MSC_VER < 1916
#       error PodSerializer library requires MSVC version 19.16 and above
#       error Compilers with versions below have some bugs, that prevent using reflection.
#   endif // _MSC_VER < 1922
#   if _MSVC_LANG < 201402L
#       error PodSerializer library requires C++14 language standard support.
#   endif // _MSVC_LANG < 201402L
#elif __cplusplus < 201402L
#   error PodSerializer library requires C++14 language standard support.
#endif // defined(_MSC_VER)


//
// Predefined language standards
// 
#define __POD_SERIALIZER_CXX14 201402L
#define __POD_SERIALIZER_CXX17 201703L


//
// Current language standard
// 
#if defined(_MSVC_LANG)
#   define __POD_SERIALIZER_LANGUAGE_VERSION _MSVC_LANG
#else
#   define __POD_SERIALIZER_LANGUAGE_VERSION __cplusplus
#endif // defined(_MSVC_LANG)


//
// Macro to put 'constexpr' keyword to some places, where it is supported in C++17,
// but not supported in C++14, e.g. 'if constexpr' statement
// 
#if ( __POD_SERIALIZER_LANGUAGE_VERSION >= __POD_SERIALIZER_CXX17 )
#   define __CONSTEXPR_SINCE_CXX17 constexpr
#else
#   define __CONSTEXPR_SINCE_CXX17 /* Not supported */
#endif // ( __POD_SERIALIZER_LANGUAGE_VERSION >= __POD_SERIALIZER_CXX17 )