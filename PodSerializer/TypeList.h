#pragma once

#include "pch.h"

#include "Tuple.h"
#include "Misc.h"

namespace type_list {

    /************************************************************************************/

    //
    // Class that represents a typelist
    // 

    template<
        typename... _Types /* Types stored in typelist */
    > struct TypeList { /* Nothing more needed here */ };

    using EmptyTypeList = TypeList<>;

    /************************************************************************************/

    //
    // Function retreives size of typelist
    // 

    template<
        typename... _Types /* Types stored in typelist */
    > constexpr size_t Size( TypeList<_Types...> )
    { 
        return sizeof...( _Types ); 
    }

    /************************************************************************************/

    //
    // Function that returns type by it's index from typelist
    // 

namespace {

    template<typename _IdxSeqType> 
    struct _get_Impl_T; /* Not implemented */

    template<size_t... _Idxs>
    struct _get_Impl_T<std::index_sequence<_Idxs...>>
    {
        //
        // This function has following signature:
        //         dummy( void*, void*, ... , void*, _Type*, ... )
        //                  ^      ^            ^       ^     ^
        //                  |      |            |       |     |
        //                  ----------------------------+-----+------ sizeof...(_Idxs) times void*
        //                                              |     |
        // Pointer to exactly _Idx's type in list -------     |
        // Other unspecified arguments (remaining types) ------
        // 
        template<typename _Type>
        static constexpr _Type dummy( 
            decltype( _Idxs, (void*)nullptr )..., _Type*, ... 
        ); /* Not implemented */
    };

} // anonymous namespace

    template<
        size_t _Idx        /* Index to ge type at */,
        typename... _Types /* Types stored in typelist */
    > constexpr auto get( TypeList<_Types...> tl )
    {
        //
        // Check if index is valid
        // 
        static_assert( _Idx < Size( tl ), "Out of range in " __FUNCTION__ );

        return Identity<
            decltype( _get_Impl_T<std::make_index_sequence<_Idx>>::dummy( (_Types*)nullptr... ) )
        >{};
    }

    /************************************************************************************/

    //
    // Constructs type of a Tuple by given typelist
    // 

    template<
        typename... _Types /* Types stored in typelist */
    > constexpr auto TupleType( TypeList<_Types...> )
    {
        //
        // Just instantiate tuple with given types
        // 
        return types::Tuple<_Types...>{};
    }

    /************************************************************************************/

    //
    // Metafunction that apples metafunction to each
    // type in type list and returns type list with
    // applied changes.
    // 

    template<
        template<typename> 
            typename _MetaFunc /* Metafunction to be applied */,
        typename...  _Types    /* Types stored in type list */
    > constexpr auto Apply( TypeList<_Types...> )
    {
        return TypeList<
            typename _MetaFunc<_Types>::type...
        >{};
    }

} // type_list