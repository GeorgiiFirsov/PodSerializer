#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Tuple.h"


namespace serialization {

    //
    // Implementation of 'operator <<' for any POD structure.
    // 

    template<
        typename _Type /* Type of structure, that we want to put into stream */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& operator<<(
        std::basic_ostream<_Char, _Traits>& stream, const _Type& obj 
    )
    {
        //
        // Convert object to tuple and then put each
        // element of tuple into our stream
        // 

        auto obj_tpl = ToTuple( obj );
        auto FlushToStream = [&stream]( auto&& element ) {
            stream << element << " ";
        };

        types::for_each( obj_tpl, FlushToStream );

        return stream;
    }

} // serialization
