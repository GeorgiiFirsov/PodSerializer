#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Buffers.h"
#include "Tuple.h"

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

 

namespace serialization {

    //
    // Here comes implementation of some serialization algorithms.
    //

    /************************************************************************************/

    //
    // Basic serialization class, that you can tune with strategies.
    // 
    template<
        typename _Type /* Type to be serialized */,
        template<typename> typename _BufferType /* Buffer type */
    > class BasicSerializer
    {
        using buffer_t = _BufferType<_Type>;

    public:
        constexpr BasicSerializer() = default;
        ~BasicSerializer() = default;

        //
        // Allow copying and moving. Trivial realization is enough.
        // 
        constexpr BasicSerializer( const BasicSerializer<_Type, _BufferType>& ) = default;
        constexpr BasicSerializer( BasicSerializer<_Type, _BufferType>&& ) = default;

        constexpr BasicSerializer<_Type, _BufferType>& 
        operator=( const BasicSerializer<_Type, _BufferType>& ) = default;

        constexpr BasicSerializer<_Type, _BufferType>&
        operator=( BasicSerializer<_Type, _BufferType>&& ) = default;

        template<
            typename... _ArgsTypes /* Optional parameters' types */
        > constexpr void Serialize( const _Type& obj, buffer_t& buffer, _ArgsTypes&&... args ) 
        { 
            buffer.Save( obj, std::forward<_ArgsTypes>( args )... ); 
        }

        template<
            typename... _ArgsTypes /* Optional parameters' types */
        > constexpr void Deserialize( _Type& obj, buffer_t& buffer, _ArgsTypes&&... args )
        {
            buffer.Load( obj, std::forward<_ArgsTypes>( args )... );
        }
    };

    /************************************************************************************/

    template<typename _Type>
    using BinarySerializer = BasicSerializer<_Type, BinaryBuffer>;

} // serialization

