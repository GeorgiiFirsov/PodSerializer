#pragma once

#include "pch.h"

#include "GetFieldsCount.h"
#include "TypeList.h"
#include "Misc.h"


/************************************************************************************
 *
 *     !!! ATTENTION !!! ATTENTION !!! ATTENTION !!! ATTENTION !!! ATTENTION !!! 
 *  
 *     This file contains arcane and sorcerer technique, that is a bit dangerous 
 *   to use in production code, because it is "write-only code" by it's definition.
 *   Moreover standardization committee doesn't like this method, but for now they
 *                       don't know, how to make it ill-formed.
 *
 *                                     ---------
 *
 * The key-concept ( as always :) ):
 * - constexpr qualified function must have definition visible in call-scope
 * - We have two structures: the first one puts a declaration of some binding function
 *   in it's scope, and the second one put a definition of the same function to the
 *   same scope.
 * - When we need to make key-value pair we just instantiate 'RegisterValue' template
 *   with specified template parameters. It instantiates a 'Key' template. That's it!
 * - To get value we just need to write folloving line of code:
 *             decltype( GetValue( Key<_TypeKey, _IdxKey>{} ) )::type
 *
 ************************************************************************************/

namespace reflection {
namespace details {

    //
    // This strcuture describes a key part of compile-time mapping.
    // 

    template<
        typename _Type /* External struct type */,
        size_t   _Idx  /* Index of field in this structure */
    > struct Key
    {
        //
        // Here we need just to put DECLARATION of
        // function, that binds key with value.
        // 
        constexpr friend 
        auto /* Because of 'auto' this function MUST have definition in the same scope */
        GetValue( 
            /* This type is empty, so there's 
             * no proplem to pass it by value */
            Key<_Type, _Idx>
        ) noexcept;
    };

    /************************************************************************************/

    //
    // This struct is used for making a binding between key and value.
    // 

    template<
        typename _TypeKey   /* External struct (first part of key) */,
        size_t   _IdxKey    /* Index in struct (second part of key) */,
        typename _TypeValue /* Type-value - the _Idx'th type in struct */,
        bool /* dummy */    /* This is a dummy value necessary for instantiation */
    > struct RegisterValue
    {
        //
        // Here we put DEFINITION of binding function.
        // 
        friend constexpr auto GetValue( Key<_TypeKey, _IdxKey> ) noexcept {
            /* We return light-weight wrapper */
            return Identity<_TypeValue>{};
        }
    };

    //
    // This specialization is necessary to avoid multiple definitions
    // 

    template<
        typename _TypeKey,
        size_t   _IdxKey,
        typename _TypeValue
    > struct RegisterValue<_TypeKey, _IdxKey, _TypeValue, true> 
    { };

    /************************************************************************************/

    //
    // Modification of _UniversalInit structure to make it work
    // with our magical key-value technique
    // 

    template<
        typename _Type /* External struct type */,
        size_t   _Idx  /* Index of field */
    > struct _KVUniversalInit // KV for key-value
    {
        template<typename /* T */, size_t /* I */> 
        static constexpr size_t _Internal( ... ) noexcept NOT_IMPLEMENTED;
        template<typename /* T */, size_t I, size_t = sizeof( GetValue( Key<_Type, I>{} ) )>
        static constexpr char _Internal( int ) noexcept NOT_IMPLEMENTED;

        template<
            typename _TypeField /* Type of field to initialize */,
            /* Magic to inject friend binding function, that can be found by ADL */
            size_t = sizeof(
                /* These three lines inject binging function by instantiating templates above */
                RegisterValue<
                    _Type, _Idx, _TypeField, sizeof( _Internal<_TypeField, _Idx>( 0 ) ) == sizeof( char )
                > 
            )
        > constexpr operator _TypeField() const noexcept NOT_IMPLEMENTED;
    };

    /************************************************************************************/

    //
    // This struct instantiates a TypeList based on structure
    // type and indices of internal fields.
    // 

    template<
        typename  _Type /* External struct type */,
        size_t... _Idxs /* Indices of fields */
    > struct _TypeList_Impl
       /* types::Tuple<size_t> */
        : types::Tuple<decltype(
            /* Injecting binding functions to construct mapping */
            _Type{ _KVUniversalInit<_Type, _Idxs>{}... }, 
            size_t{ 0 } 
        )>
    {
        using tlist = decltype(
            /* Here we unwind one Identity level, because 
             * 'GetValue' returns 'Identity<_TypeValue>' */
            type_list::Apply<_UnwindIdentity>( 
                type_list::TypeList<decltype( GetValue( Key<_Type, _Idxs>{} ) )...>{} 
            )
        );
    };

    /************************************************************************************/

    //
    // This helper-function helps to avoid ugly and scary code by
    // automatically deducing indices and passing all the parameters
    // to '_TypeList_Impl' and returning constructed type.
    // 

    template<
        typename  _Type /* External struct type */,
        size_t... _Idxs /* Indices of fields */
    > constexpr auto _GetTypeList_Impl( std::index_sequence<_Idxs...> ) noexcept
    {
        using tlist_t = typename _TypeList_Impl<_Type, _Idxs...>::tlist;
        return tlist_t{};
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<
        typename _Type /* Structure to get internal types from */
    > constexpr
    auto /* We return TypeList, but we dont know about it's internal types */
    GetTypeList() noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE_EXTENDED( _CleanType );

        return details::_GetTypeList_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

    template<
        typename _Type /* Structure to get internal types from */
    > constexpr auto GetTypeList(
        const _Type& /* obj */ /* For implicit template parameter deduction */
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE_EXTENDED( _CleanType );

        return details::_GetTypeList_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

} // reflection