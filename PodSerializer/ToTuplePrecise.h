#pragma once

#include "pch.h"

#include "GetTypeList.h"
#include "TypeList.h"


/************************************************************************************
 * ToTuplePrecise function
 * 
 * The key-concept is following:
 *  - The difference between ToTuplePrecise and ToTuple is that the first one
 *    returns exact types without recursive looking inside internal types.
 *    
 ************************************************************************************/


namespace reflection {
namespace details {

    template<
        typename _Type /* Type to convert into a tuple */
    > constexpr decltype(auto) _ToTuplePrecise_Impl(
        const _Type& obj /* Object to convert into tuple */
    )
    {
        using tlist_t = decltype( GetTypeList<_Type>() );
        using tuple_t = decltype( TupleType( std::declval<tlist_t>() ) );

        //
        // As soon as here tuple has the same
        // layout as our struct, so we can just
        // convert struct into a tuple.
        // 

        auto pObj = static_cast<const void*>( &obj );
        return *static_cast<const tuple_t*>( pObj );
    }

} // namespace details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<typename _Type> 
    constexpr decltype(auto) ToTuplePrecise(
        const _Type& obj /* Object to convert into tuple */
    )
    {
        using _CleanType = std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE_EXTENDED( _CleanType );

        return details::_ToTuplePrecise_Impl( obj );
    }

} // namespace reflection
