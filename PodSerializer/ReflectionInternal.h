#pragma once

#include "pch.h"

#include "Utils.h"
#include "Tuple.h"
#include "SizeTArray.h"

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
        using types::SizeTArray;
        using utils::_IndexedUniversalInit;

        constexpr SizeTArray<sizeof...( _Idxs )> ids{ { 0 } };

        //
        // Here we write ids into array by creating temporary object.
        // 
        _Type temporary{ _IndexedUniversalInit<_Idxs>{ const_cast<size_t*>( ids.data ) }... };
        return ids;
    }

    /************************************************************************************/

    template<
        typename  _Type /* Type to convert into a tuple */,
        size_t... _Idxs /* Indices */
    > constexpr auto _ToTuple_Impl( 
        _Type& obj /* Object to convert into a structure */, 
        std::index_sequence<_Idxs...> /* indices */ 
    )
    {
        using utils::SizeT;
        using utils::_GetTypeById;
        using types::Tuple;
        using types::get;

        // 
        // Array with ids of types in structure. It is important
        // to ensure it to be constexpr.
        // 
        constexpr auto ids = _GetTypeIds_Impl<_Type>( 
            std::make_index_sequence<sizeof...( _Idxs )>{} 
        );

        using return_t = Tuple<
            /* Walk through all saved ids of tupes and convert back them to types.
             * Until this time it was extremely important to do all our manipulations
             * in compile-time, bacause types, passes to tuple should be known now. */
            decltype( _GetTypeById( SizeT<get<_Idxs>( ids )>{} ) )...
        >;

        //
        // Bad black magic }-)
        // Just trust me here - tuple and passed structure have the same layout,
        // but can not be casted to each other directly.
        // 
        return *(return_t*)( &obj );
    }

    /************************************************************************************/

    template<
        typename    _Type  /* Type to consruct from tuple */,
        typename... _Types /* Types stored in tuple */,
        size_t...   _Idxs  /* Indices */
    > constexpr _Type _FromTuple_Impl( 
        const types::Tuple<_Types...>& tpl /* Tuple to be converted into struct */, 
        std::index_sequence<_Idxs...> /* indices */ 
    ) noexcept( std::is_nothrow_constructible<_Type, _Types...>::value )
    {
        using types::get;
        using reflection::GetFieldsCount;

        static_assert( 
            sizeof...( _Idxs ) == sizeof...( _Types ), 
            "Types and indices amounts mismatch in " __FUNCTION__ 
        );

        //
        // Simply construct object using aggregate initialization.
        // 
        return _Type{ get<_Idxs>( tpl )... };
    }

} // details
} // reflection