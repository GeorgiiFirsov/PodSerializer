#pragma once

#include "pch.h"

#include "Support.h"
#include "Tuple.h"


/************************************************************************************
 * FromTuple function
 * 
 * The key-concept is following:
 *  - Walk through all tuple elements using pack-expansion and initialize return
 *    value with values of these elements.
 *    
 ************************************************************************************/


namespace reflection {
namespace details {

    /************************************************************************************/

    //
    // Stucture used to wrap reference to variable of one type
    // and then assign to variable of any type (types must be convertible to each other).
    // 
    template<typename _Stored>
    struct TypeCaster
    {
        // 
        // Reference to external value
        // 
        _Stored& value;

        template<typename _Type>
        constexpr operator _Type() const noexcept
        {
            //
            // _Stored and _Type could be the same types and 'static_cast' does nothing.
            // But _Stored can be uderlying type of enumeration of type _Type. That is 
            // the reason, why it is necessary to use 'static_cast' here.
            // 
            return static_cast<_Type>( value );
        }
    };

    /************************************************************************************/

    template<
        typename    _Type  /* Type to consruct from tuple */,
        typename... _Types /* Types stored in tuple */,
        size_t...   _Idxs  /* Indices of internal types */
    > constexpr _Type _FromTuple_Impl( 
        const types::Tuple<_Types...>& tpl /* Tuple to be converted into struct */, 
        std::index_sequence<_Idxs...> /* indices */ 
    ) noexcept
    {
        using reflection::GetFieldsCount;
        using types::get;

        static_assert( 
            sizeof...( _Idxs ) == sizeof...( _Types ), 
            "Types and indices amounts mismatch in " __FUNCTION__ 
        );

        //
        // Simply construct object using aggregate initialization.
        // 
        return _Type{ 
            TypeCaster<decltype( get<_Idxs>( tpl ) )>{ get<_Idxs>( tpl ) }... 
        };
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<
        typename    _Type  /* Type to construct from tuple */, 
        typename... _Types /* Types stored in tuple */
    >
    constexpr _Type FromTuple(
        const types::Tuple<_Types...>& tpl
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE_EXTENDED( _CleanType );

        return details::_FromTuple_Impl<_Type>( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

} // reflection