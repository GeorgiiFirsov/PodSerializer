#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Buffers.h"
#include "Tuple.h"
 

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

    //
    // Binary serializer alias
    // 

    template<typename _Type>
    using BinarySerializer = BasicSerializer<_Type, BinaryBuffer>;

    /************************************************************************************/

    //
    // Stream serializer aliases
    // 

    template<typename _Type>
    using StringStreamSerializer = BasicSerializer<_Type, StringStreamBuffer>;

    template<typename _Type>
    using WStringStreamSerializer = BasicSerializer<_Type, WStringStreamBuffer>;

} // serialization

