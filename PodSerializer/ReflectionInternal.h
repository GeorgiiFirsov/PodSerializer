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
    > constexpr auto _GetTypeIds_Impl( std::index_sequence<_Idxs...> indices ) 
        noexcept( std::is_nothrow_constructible<_Type, utils::_UniversalInit<_Idxs>...>::value )
    {
        using utils::_IndexedUniversalInit;
        using utils::_GetIdsRaw_Impl;
        using utils::ArrayTransformer;
        using types::SizeTArray;

        //
        // Raw array with potential zero paddings
        // 
        constexpr auto idsRaw = _GetIdsRaw_Impl<_Type>( indices );

        //
        // Here we need to remove all zeros from 'idsRaw' array.
        // Use helper class ArrayTransformer.
        // 
        constexpr SizeTArray<sizeof...( _Idxs )> idsWithoutZeros{ { 0 } };

        constexpr ArrayTransformer<sizeof( _Type )> transform{ 
            const_cast<size_t*>( idsRaw.data ), 
            const_cast<size_t*>( idsWithoutZeros.data ) 
        };
        transform.Run();

        return idsWithoutZeros;
    }

    /************************************************************************************/

    template<
        typename  _Type /* Type to convert into a tuple */,
        size_t... _Idxs /* Indices */
    > constexpr auto _ToTuple_Impl( 
        _Type& obj /* Object to convert into a structure */, 
        std::index_sequence<_Idxs...> /* indices */ 
    ) noexcept
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

        using tuple_t = Tuple<
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
        auto pObj = static_cast<const void*>( &obj );
        return *static_cast<const tuple_t*>( pObj );
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
        using reflection::GetFieldsCount;
        using utils::TypeCaster;
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
} // reflection