#pragma once

#include "pch.h"

#include "Support.h"
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
    > constexpr size_t GetFieldsCount() noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetFieldsCount_Impl<_CleanType>( 
            std::make_index_sequence<sizeof( _CleanType )>{} 
        );
    }

    template<
        typename _Type /* Type to count fields in */
    > constexpr size_t GetFieldsCount( 
        const _Type& /* obj */ /* For implicit template parameter deduction */ 
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetFieldsCount_Impl<_CleanType>( 
            std::make_index_sequence<sizeof( _CleanType )>{} 
        );
    }


    /************************************************************************************
     * GetTypeIds function
     * 
     * The key-concept is following:
     *  - We register all fundamental types explicitly
     *  - Due to registration we have mapping between types and integers
     *  - These functions return compile-time arrays of those integers.
     *    
     ************************************************************************************/

    template<
        typename _Type /* Type to convert into ids array */
    > constexpr decltype(auto) GetTypeIds() noexcept( std::is_nothrow_constructible<_Type>::value )
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetTypeIds_Impl<_CleanType>( 
            std::make_index_sequence<GetFieldsCount<_CleanType>()>{} 
        );
    }

    template<
        typename _Type /* Type to convert into ids array */
    > constexpr decltype(auto) GetTypeIds(
        const _Type& /* obj */ /* For implicit template parameter deduction */
    ) noexcept( std::is_nothrow_constructible<_Type>::value )
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetTypeIds_Impl<_CleanType>( 
            std::make_index_sequence<GetFieldsCount<_CleanType>()>{} 
        );
    }


    /************************************************************************************
     * ToTuple function
     * 
     * The key-concept is following:
     *  - Convert built inside array of ids of types into types back and use them to 
     *    initialize tuple
     *    
     ************************************************************************************/

    template<typename _Type> 
    constexpr /* Maybe it is possible to construct our tuple in compile-time */
    decltype(auto) ToTuple( 
         const _Type& obj /* Object to convert into tuple */
    )
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_ToTuple_Impl( 
            obj, std::make_index_sequence<GetFieldsCount<_CleanType>()>{} 
        );
    }


    /************************************************************************************
     * FromTuple function
     * 
     * The key-concept is following:
     *  - Walk through all tuple elements using pack-expansion and initialize return
     *    value with values of these elements.
     *    
     ************************************************************************************/

    template<typename _Type, typename... _Types>
    constexpr _Type FromTuple(
        const types::Tuple<_Types...>& tpl
    ) noexcept( std::is_nothrow_constructible<_Type, _Types...>::value )
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_FromTuple_Impl<_CleanType>( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

} // reflection