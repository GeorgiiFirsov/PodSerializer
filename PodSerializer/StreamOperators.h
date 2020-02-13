#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Tuple.h"

namespace operators {
namespace {

    //
    // Internal implementation of 'operator >>'
    // 

    template<
        size_t... _Idxs /* Indices */,
        typename  _Type /* Type of structure, that we want to get from stream */,
        typename  _Char /* Type of chars used in stream */,
        typename  _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& _OperatorFromStream_Impl(
        std::basic_istream<_Char, _Traits>& stream /* Stream to read from */, 
        _Type& obj /* Object that used to store data from stream */, 
        std::index_sequence<_Idxs...> /* indices */ 
    )
    {
        using reflection::details::_GetTypeById;
        using reflection::details::SizeT;
        using reflection::GetTypeIds;
        using reflection::FromTuple;
        using types::Tuple;
        using types::get;

        //
        // Determine type of a tuple
        // 
        constexpr auto ids = GetTypeIds<_Type>();
        using tuple_t = Tuple< decltype( _GetTypeById( SizeT<get<_Idxs>( ids )>{} ) )... >;

        //
        // Create one and put values from stream type-safely into it
        // 
        tuple_t container;

        auto PutToTuple = [&stream = stream]( auto& /* non-const lvalue */ element ) 
        {
            stream >> element;
        };

        types::for_each( container, PutToTuple );

        obj = FromTuple<_Type>( container );

        return stream;
    }

} // anonymous namespace

    /************************************************************************************/

    //
    // Implementation of 'operator >>' for any POD structure.
    //
    template<
        typename _Type /* Type of structure, that we want to get from stream */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& operator>>(
        std::basic_istream<_Char, _Traits>& stream, _Type& obj
    )
    {
        return _OperatorFromStream_Impl( 
            stream, obj, std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

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
            stream << std::forward<decltype( element )>( element ) << ' ';
        };

        types::for_each( obj_tpl, FlushToStream );

        return stream;
    }

} // operators