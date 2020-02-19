#pragma once

#if defined(_MSC_VER)
#   if _MSC_VER < 1916
#       error PodSerializer library requires MSVC version 19.16 and above
#       error Compilers with versions below have some bugs, that prevent using reflection.
#   endif // _MSC_VER < 1922
#elif __cplusplus < 201402L
#   error PodSerializer library requires C++14 language standard support.
#endif // defined(_MSC_VER)
