#pragma once

#include "pch.h"

/************************************************************************************
 * Motivation to implement own tuple.
 * 
 * We need to store values in tuple in order that they are listed in.
 * C++ standard tells us nothing about internal order of values in std::tuple.
 * For instance, in Microsoft implementation of STL values in tuple are stored
 * in inverse order. It doesn't match our requirements.
 *
 ************************************************************************************/

namespace types {
namespace {

    //
    // Internal implementation.
    // Tuple and get are listed after this anonymous namespace.
    // 

    /************************************************************************************/

    //
    // Storage for value in tuple.
    // 
    template<
        size_t /* _Idx */,
        typename _Type
    > struct _ValueContainer {
        _Type value;
    };

    template<typename _IdxsType, typename... _TailTypes> 
    struct _Tuple_Impl; /* Not implemented. Don't worry about it :) */

    //
    // Yet comes implementation of our tuple
    // 
    template<
        size_t...   _Idxs      /* Indices */,
        typename... _TailTypes /* Tail of typelist */
    > struct _Tuple_Impl<std::index_sequence<_Idxs...>, _TailTypes...>
        /* Instatiate conatiner of a value with each
         * type and inherit from all of them. */
        : _ValueContainer<_Idxs, _TailTypes>...
    {
        static constexpr size_t size = sizeof...( _Idxs );

        // Provide some constructors
        constexpr _Tuple_Impl()                     = default;
        constexpr _Tuple_Impl( const _Tuple_Impl& ) = default;
        constexpr _Tuple_Impl( _Tuple_Impl&& )      = default;

        constexpr _Tuple_Impl( _TailTypes... values ) noexcept
            : _ValueContainer<_Idxs, _TailTypes>{ values }...
        { }
    };

    template<>
    struct _Tuple_Impl<std::index_sequence<>>
    {
        static constexpr size_t size = 0;
    };

    /************************************************************************************/

    //
    // 'get' implementation. 
    // It is necessary to implement it for all modifications of _Type.
    // 
    
    // lvalue reference
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr _Type& _get_Impl( _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }

    // const lvalue reference
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr const _Type& _get_Impl( const _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }
    
    // volatile lvalue reference
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr volatile _Type& _get_Impl( volatile _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }

    // const volatile lvalue reference
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr const volatile _Type& _get_Impl( const volatile _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }

    // rvalue reference
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr _Type&& _get_Impl( _ValueContainer<_Idx, _Type>&& container ) noexcept
    {
        return container.value;
    }

} // anonymous namespace


    //
    // Tuple and get to use in application
    // 
    template<typename... _Types>
    struct Tuple
        : _Tuple_Impl<
            std::make_index_sequence<sizeof...( _Types )>, 
            _Types...
        >
    {
        using _Tuple_Impl<
            std::make_index_sequence<sizeof...(_Types)>,
            _Types...
        >::_Tuple_Impl;
    };

    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( const Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( volatile Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( const volatile Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( Tuple<_Types...>&& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

} // types
