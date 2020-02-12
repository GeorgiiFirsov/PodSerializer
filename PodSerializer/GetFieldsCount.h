#pragma once

#include "pch.h"

#include "Support.h"


/************************************************************************************
 * GetFieldsCount function
 * 
 * The key-concept is following:
 *  - _Type{ val1, val2, ..., valn } will be compiled successfully only in case, 
 *    when n is less or equal than amount of fields in structure _Type.
 *  - We can use SFINAE idiom to detect the moment of equality of n and amount of 
 *    fields. We just need to start from one big amount of fields and to keep 
 *    substitute corresponding amount of values into a aggregate initialization 
 *    until we succeed.
 *  - Maximum amount of fields in structure is sizeof( _Type ), because it 
 *    corresponds to structure with only fields of type char (or another byte-sized 
 *    type). Here we don't expect bit fields.
 *    
 ************************************************************************************/


namespace reflection {
namespace details {

    /************************************************************************************/

    //
    // Structure used to "initialize" any variable of any type.
    // 
    template<
        size_t /* _Idx */ /* Formal index that makes easier work with variadics */
    > struct _UniversalInit
    {
        template<
            typename _Type /* Type to be initialized */
        > constexpr operator _Type&() const noexcept; // Not implemented - it's ok :)
    };

    /************************************************************************************/

    //
    // Function template that returns amount of _Type's fields.
    // This template is more specialized => it must win overload
    // resolution to the next one below.
    // 
    template<
        typename  _Type /* Type to count fields in */,
        size_t    _Idx  /* First index */,
        size_t... _Idxs /* Rest indices of internal types inside _Type */
    > constexpr auto _GetFieldsCount_Impl( std::index_sequence<_Idx, _Idxs...> /* indices */ ) noexcept
        -> decltype
        ( 
            /* Attempt to aggregate initialization with specified amount of params.
             * Failure is not threated as error due to SFINAE. Initialization can fail
             * only in case when amount of fields is less than items in aggregate
             * initialization list. Otherwise initialization succeeds. */
            _Type{ _UniversalInit<_Idx>{}, _UniversalInit<_Idxs>{}... }, 

            /* But as return type we use size_t (comma operator used here) */
            size_t{}
        )
    {
        //
        // In case of successful instantiation of function 
        // template we receive type's fields count.
        // 
        return sizeof...( _Idxs ) + 1;
    }

    //
    // Function template that reduces amount of parameters by one.
    // 
    template<
        typename  _Type /* Type to count fields in */,
        size_t... _Idxs /* Indices of internal types */
    > constexpr size_t _GetFieldsCount_Impl( std::index_sequence<_Idxs...> /* indices */ ) noexcept
    {
        //
        // First template was instantiated unsuccessfully,
        // Reducing parameters amount by one and trying again.
        // 
        return _GetFieldsCount_Impl<_Type>( 
            std::make_index_sequence<sizeof...( _Idxs ) - 1>{} 
        );
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<
        typename _Type /* Type to count fields in */
    > constexpr size_t GetFieldsCount() noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetFieldsCount_Impl<_Type>( 
            std::make_index_sequence<sizeof( _Type )>{} 
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

        return details::_GetFieldsCount_Impl<_Type>( 
            std::make_index_sequence<sizeof( _Type )>{} 
        );
    }

} // reflection