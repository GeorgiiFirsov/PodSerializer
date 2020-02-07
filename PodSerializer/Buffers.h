#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Tuple.h"


namespace serialization {

    /************************************************************************************/

    //
    // Buffer for binary serialization
    // 

    template<
        typename _Type /* Type to be stored */
    > class BinaryBuffer
    {
        using buffer_t = std::vector<unsigned char>;
        using value_t = _Type;

    public:
        BinaryBuffer()
            : m_isFull( false )
            , m_buffer( buffer_t( sizeof( value_t ), 0 ) )
        { }

        BinaryBuffer( const BinaryBuffer<_Type>& ) = default;
        BinaryBuffer& operator=( const BinaryBuffer<_Type>& ) = default;

        BinaryBuffer( BinaryBuffer<_Type>&& ) = default;
        BinaryBuffer& operator= ( BinaryBuffer<_Type>&& ) = default;

        bool IsEmpty()
        {
            return !m_isFull;
        }

        void Clear()
        {
            m_isFull = false;
            m_buffer.assign( sizeof( value_t ), 0 );
        }

        void Save( const value_t& obj )
        {
            using reflection::ToTuple;

            if (m_isFull) {
                Clear();
            }

            //
            // Offset is necessary for memorizing location
            // in buffer to write in.
            // 
            size_t offset = 0;

            //
            // Now we convert our structure into a tuple
            // and then walk through all fields with serializing.
            // 
            auto obj_tpl = ToTuple( obj );

            auto SaveToBuffer = [&offset, buffer = m_buffer.data()]( auto&& element )
            {
                memcpy( buffer + offset, &element, sizeof( element ) );
                offset += sizeof( element );
            };

            types::for_each( obj_tpl, SaveToBuffer );

            m_isFull = true;
        }

        void Load( value_t& obj )
        {
            using reflection::ToTuple;
            using reflection::FromTuple;

            //
            // Check if buffer contains a value.
            // 
            if (IsEmpty()) {
                throw std::logic_error( "Buffer is empty" );
            }

            using tuple_t = decltype( ToTuple( obj ) );

            //
            // Another black magic }-)
            // 
            tuple_t temporary = *(tuple_t*)( m_buffer.data() );

            obj = FromTuple<value_t>( temporary );
        }

    private:

        //
        // Flag of emptiness 
        // 
        bool m_isFull;

        //
        // Internal buffer
        // 
        buffer_t m_buffer;
    };
}

