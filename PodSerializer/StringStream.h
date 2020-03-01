#pragma once

#include "pch.h"


namespace io_stream {
    
    //
    // This stream is utility class, because it makes possible
    // to serialize std::basic_string into a StringStreamSerializer.
    // If we use standard string_stream, we are unable to load strings
    // that contains spaces.
    // 

    template<
        typename _Char /* character type inside stringstream */,
        typename _Traits = std::char_traits<_Char>,
        typename _Allocator = std::allocator<_Char>
    > class BasicStringStream
        : public std::basic_stringstream<_Char, _Traits, _Allocator>
    { /* We don't change anything here */ };

    //
    // Ovreload of operator>> for strings, that is insensetive to spaces
    // 

    template<
        typename _Char /* character type inside stringstream and string */,
        typename _Traits = std::char_traits<_Char>,
        typename _Allocator = std::allocator<_Char>
    > BasicStringStream<_Char, _Traits, _Allocator>& operator>>(
        BasicStringStream<_Char, _Traits, _Allocator>& stream, 
        std::basic_string<_Char, _Traits, _Allocator>& str
    )
    {
        std::getline( stream, str );
        return stream;
    }

} // namespace io_stream