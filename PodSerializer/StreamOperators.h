#pragma once

#include "pch.h"

#include "Config.h"

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

    /************************************************************************************/

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
        using reflection::ToTuplePrecise;

        //
        // Convert object to tuple and then put each
        // element of tuple into our stream
        // 
        
        if (stream.iword( bs_fmt_id ) == flags::beautiful_struct) stream << "{ ";

        auto obj_tpl = ToTuplePrecise( obj );
        size_t index = 0;

        auto FlushToStream = [&stream, &index]( auto&& element ) 
        {
            if (index++) stream << (stream.iword( bs_fmt_id ) == flags::beautiful_struct ? ", " : " ");
            stream << std::forward<std::remove_reference<decltype( element )>::type>( element );
        };

        types::for_each( obj_tpl, FlushToStream );

        if (stream.iword( bs_fmt_id ) == flags::beautiful_struct) stream << " }";

        return stream;
    }

    /************************************************************************************/

    static int const bs_fmt_id = std::ios_base::xalloc();

namespace flags {
    enum _Flags
    {
        no_beautiful_struct = 0,
        beautiful_struct = 1
    };
} //namespace flags

    template<
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& operator<<(
        std::basic_ostream<_Char, _Traits>& stream, flags::_Flags flag
    )
    {
        stream.iword( bs_fmt_id ) = flag;
        return stream;
    }

} // operators
