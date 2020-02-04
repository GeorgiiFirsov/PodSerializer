#pragma once

#include "pch.h"

#include "Utils.h"

namespace reflection {
namespace details {

    /************************************************************************************/

    //
    // Function template that writes amount of _Type's fields into count parameter
    // 
    template<
        typename  _Type /* Type to count fields in */,
        size_t    _Idx  /* First index */,
        size_t... _Idxs /* Rest indices */
    > constexpr auto _GetFieldsCount_Impl( size_t& count, std::index_sequence<_Idx, _Idxs...> /* indices */ )
        noexcept( std::is_nothrow_constructible<_Type>::value )
        -> decltype( 
            /* Attempt to aggregate initialization with specified amount of params.
             * Failure is not threated as error due to SFINAE. Initialization can fail
             * only in case when amount of fields is less than items in aggregate
             * initialization list. Otherwise initialization succeeds. */
            _Type{ utils::_UniversalInit<_Idx>{}, utils::_UniversalInit<_Idxs>{}... } 
        )
    {
        //
        // In case of successful instantiation of function 
        // template we receive type's fields count.
        // 
        count = sizeof...( _Idxs ) + 1;
        return _Type{};
    }

    //
    // Function template that reduces amount of parameters by one.
    // 
    template<
        typename  _Type /* Type to count fields in */,
        size_t... _Idxs /* Indices */
    > constexpr void _GetFieldsCount_Impl( size_t& count, std::index_sequence<_Idxs...> /* indices */ )
        noexcept( 
            /* At the end of recursion noexcept specifier will depend on type _Type */
            std::is_nothrow_constructible<_Type>::value 
        )
    {
        //
        // First template instantiated unsuccessfully,
        // Reducing parameters amount by one and trying again.
        // 
        _GetFieldsCount_Impl<_Type>( 
            count, std::make_index_sequence<sizeof...( _Idxs ) - 1>{} 
        );
    }

    /************************************************************************************/

} // details
} // reflection