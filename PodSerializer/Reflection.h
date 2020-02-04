#pragma once

#include "pch.h"

#include "ReflectionInternal.h"

namespace reflection {

    /************************************************************************************
     * GetFieldsCount function
     * 
     * The key-concept is following:
     *  - Type{ val1, val2, ..., valn } will be compiled successfully only in case, 
     *    when n is less or equal than amount of fields in structure Type.
     *  - We can use SFINAE idiom to detect the moment of equality of n and amount of 
     *    fields. We just need to start from one big amount of fields and to keep 
     *    substitute corresponding amount of values into a aggregate initialization 
     *    until we succeed.
     *  - Maximum amount of fields in structure is sizeof( Type ), because it 
     *    corresponds to structure with only fields of type char (or another byte-sized 
     *    type). Here we don't expect bit fields etc.
     *    
     ************************************************************************************/

    template<
        typename _Type /* Type to count fields in */
    > constexpr size_t GetFieldsCount()
        noexcept
    {
        REFLECTION_CHECK_TYPE( _Type );

        size_t result;
        details::_GetFieldsCount_Impl<_Type>( 
            result, std::make_index_sequence<sizeof( _Type )>{} 
        );
        return result;
    }

    template<
        typename _Type /* Type to count fields in */
    > constexpr size_t GetFieldsCount( 
        const _Type& /* obj */ /* For implicit template parameter deduction */ 
    )
        noexcept
    {
        REFLECTION_CHECK_TYPE( _Type );

        size_t result;
        details::_GetFieldsCount_Impl<_Type>( 
            result, std::make_index_sequence<sizeof( _Type )>{} 
        );
        return result;
    }

} // reflection