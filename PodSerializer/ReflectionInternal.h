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
    > constexpr auto _GetFieldsCount_Impl( std::index_sequence<_Idx, _Idxs...> indices ) noexcept
        -> decltype
        ( 
            /* Attempt to aggregate initialization with specified amount of params.
             * Failure is not threated as error due to SFINAE. Initialization can fail
             * only in case when amount of fields is less than items in aggregate
             * initialization list. Otherwise initialization succeeds. */
            _Type{ utils::_UniversalInit<_Idx>{}, utils::_UniversalInit<_Idxs>{}... }, 

            /* But as return type we use size_t */
            use_as_return_type( size_t )
        )
    {
        //
        // In case of successful instantiation of function 
        // template we receive type's fields count.
        // 
        return indices.size();
    }

    //
    // Function template that reduces amount of parameters by one.
    // 
    template<
        typename  _Type /* Type to count fields in */,
        size_t... _Idxs /* Indices */
    > constexpr size_t _GetFieldsCount_Impl( std::index_sequence<_Idxs...> /* indices */ ) noexcept
    {
        //
        // First template instantiated unsuccessfully,
        // Reducing parameters amount by one and trying again.
        // 
        return _GetFieldsCount_Impl<_Type>( 
            std::make_index_sequence<sizeof...( _Idxs ) - 1>{} 
        );
    }

    /************************************************************************************/

    template<
        typename  _Type  /* Type to return ids for */,
        size_t... _Idxs  /* Indices */
    > constexpr auto _GetTypeIds_Impl( std::index_sequence<_Idxs...> /* indices */ ) 
        noexcept( std::is_nothrow_constructible<_Type>::value )
    {
        constexpr utils::SizeTArray<sizeof...( _Idxs )> ids{ { 0 } };

        //
        // Here we write ids into array by creating temporary object.
        // 
        _Type temporary{ utils::_IndexedUniversalInit<_Idxs>{ const_cast<size_t*>( ids.data ) }... };
        return ids;
    }

} // details
} // reflection