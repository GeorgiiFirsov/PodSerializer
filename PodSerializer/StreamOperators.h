#pragma once

#include "pch.h"

#include "Config.h"

#include "IOManipulators.h"
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
        using reflection::ToTuplePrecise;
        using reflection::FromTuple;
        using types::Tuple;
        using types::get;

        //
        // Determine type of a tuple
        // 
        using tuple_t = decltype( ToTuplePrecise( obj ) );

        auto sep = static_cast<_Char>( stream.iword( io_manipulators::io_internal::sp_fmt_id ) );
        bool bIsSepSet = stream.iword( io_manipulators::io_internal::sp_set_fmt_id ) == io_manipulators::io_internal::separator_enabled;

        //
        // Create one and put values from stream type-safely into it
        // 
        tuple_t container;
        size_t index = 0;

        auto PutToTuple = [&stream = stream, &index, sep, bIsSepSet]( auto& /* non-const lvalue */ element ) 
        {
            using _CleanType = std::decay<decltype( element )>::type;

            //
            // If type is supported to be reflected,
            // we need to call operator >> for it directly.
            // Otherwise we need to read stream until we
            // meet separator (if set).
            // 

            if (bIsSepSet && !is_supported_type_extended<_CleanType>::value)
            {
                std::basic_stringstream<_Char, _Traits> buffer;

                //
                // Read char-by-char into temporary buffer until
                // we reach separator.
                // 
                while (true)
                {
                    _Char ch;
                    stream.get( ch );
                    if (ch == sep || stream.eof()) break;
                    buffer << ch;
                }

                buffer >> std::noskipws >> element;
            }
            else
            {
                stream >> std::noskipws >> element;
            }
        };

        types::for_each( container, PutToTuple );

        obj = FromTuple<_Type>( container );

        return stream;
    }

    /************************************************************************************/

    //
    // (At least MSVC 19.22)
    // Enumeration types can not be implicitly converted
    // into fundamental underlying type, so we need to
    // dispatch it manually.
    // 

    //
    // Overload for structures
    // 

    template<
        typename _Type /* Type of structure, that we want to get from stream */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& _IstreamEnumDispatch(
        std::basic_istream<_Char, _Traits>& stream, _Type& obj, std::false_type
    )
    {
        return _OperatorFromStream_Impl( 
            stream, obj, std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

    //
    // Overload for enumerations
    // 

    template<
        typename _Type /* Enumeration type */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& _IstreamEnumDispatch(
        std::basic_istream<_Char, _Traits>& stream, _Type& obj, std::true_type
    )
    {
        using actual_t = typename std::underlying_type<_Type>::type;

        actual_t value;
        stream >> value;

        obj = static_cast<_Type>( value );

        return stream;
    }

    /************************************************************************************/

    //
    // Overload for structures
    // 

    template<
        typename _Type /* Type of structure, that we want to get from stream */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& _OstreamEnumDispatch(
        std::basic_ostream<_Char, _Traits>& stream, const _Type& obj, std::false_type
    )
    {
        using reflection::ToTuplePrecise;

        //
        // Convert object to tuple and then put each
        // element of tuple into our stream
        // 
        
        auto sep = static_cast<_Char>( stream.iword( io_manipulators::io_internal::sp_fmt_id ) );
        bool bIsSepSet = stream.iword( io_manipulators::io_internal::sp_set_fmt_id ) == io_manipulators::io_internal::separator_enabled;
        bool bIsBeautiful = stream.iword( io_manipulators::bs_fmt_id ) == io_manipulators::flags::beautiful_struct;

        if (bIsBeautiful) {
            stream << "{ ";
        }

        auto obj_tpl = ToTuplePrecise( obj );
        size_t index = 0;

        auto FlushToStream = [&stream, &index, bIsBeautiful, sep, bIsSepSet]( auto&& element ) 
        {
            if (index++) 
            {
                if (bIsSepSet) {
                    stream << sep;
                }
                else {
                    stream << (bIsBeautiful ? ", " : " ");
                }
            }

            stream << std::forward<std::remove_reference<decltype( element )>::type>( element );
        };

        types::for_each( obj_tpl, FlushToStream );

        if (bIsBeautiful) {
            stream << " }";
        }

        return stream;
    }

    //
    // Overload for enumerations
    // 

    template<
        typename _Type /* Type of structure, that we want to get from stream */,
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& _OstreamEnumDispatch(
        std::basic_ostream<_Char, _Traits>& stream, const _Type& obj, std::true_type
    )
    {
        using actual_t = typename std::underlying_type<_Type>::type;

        auto value = static_cast<actual_t>( obj );
        stream << value;

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
        return _IstreamEnumDispatch( stream, obj, std::is_enum<_Type>{} );
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
        return _OstreamEnumDispatch( stream, obj, std::is_enum<_Type>{} );
    }

    /************************************************************************************/

    using io_manipulators::flags::beautiful_struct;
    using io_manipulators::flags::no_beautiful_struct;

} // operators
