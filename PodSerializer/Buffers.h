#pragma once

#include "pch.h"

#include "Reflection.h"
#include "Tuple.h"
#include "Utils.h"


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
            using reflection::GetFieldsCount;

            //
            // Check if buffer contains a value.
            // 
            if (IsEmpty()) {
                throw std::logic_error( "Buffer is empty" );
            }

            _Load_Impl( 
                obj, std::make_index_sequence<GetFieldsCount<value_t>()>{} 
            );
        }

    private:
        template<
            size_t... _Idxs /* Indices */
        > void _Load_Impl( value_t& obj, std::index_sequence<_Idxs...> /* indices */ )
        {
            using reflection::utils::SizeT;
            using reflection::utils::_GetTypeById;
            using reflection::GetTypeIds;
            using reflection::FromTuple;
            using types::Tuple;
            using types::get;

            //
            // Here we need to know types, stored in storage
            // 
            constexpr auto ids = GetTypeIds<value_t>();
            using tuple_t = Tuple< decltype( _GetTypeById( SizeT<get<_Idxs>( ids )>{} ) )... >;

            //
            // Now put stored values into tuple
            // 
            tuple_t buffer_view;
            size_t offset = 0;

            auto PutToTuple = [&offset, buffer = m_buffer.data()]( auto& /* non-const lvalue!!! */ element )
            {
                using element_t = typename std::decay<decltype( element )>::type;

                element = *reinterpret_cast<element_t*>( buffer + offset );
                offset += sizeof( element );
            };

            types::for_each( buffer_view, PutToTuple );

            //
            // And finally convert tuple into an object.
            // 
            obj = FromTuple<value_t>( buffer_view );
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

