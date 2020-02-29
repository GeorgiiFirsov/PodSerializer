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

    //
    // Forward declaration
    // 
    template<typename...> struct Tuple;

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
    
    //
    // lvalue reference
    // 
    template<
        size_t   _Idx  /* Index of element */,
        typename _Type /* Type of element */
    > constexpr _Type& _get_Impl( _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        //
        // Just return stored value.
        // 
        return container.value;
    }

    //
    // const lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename _Type> 
    constexpr const _Type& _get_Impl( const _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }
    
    //
    // volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename _Type> 
    constexpr volatile _Type& _get_Impl( volatile _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }

    //
    // const volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename _Type> 
    constexpr const volatile _Type& _get_Impl( const volatile _ValueContainer<_Idx, _Type>& container ) noexcept
    {
        return container.value;
    }

    //
    // rvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename _Type> 
    constexpr _Type&& _get_Impl( _ValueContainer<_Idx, _Type>&& container ) noexcept
    {
        return container.value;
    }

    /************************************************************************************/

    //
    // 'ToStdTuple' implementation. 
    // It is necessary to implement it for all modifications of _Type.
    // 

    //
    // lvalue reference
    // 
    template<
        size_t...   _Idxs  /* Indices of stored types and values */,
        typename... _Types /* Types stored in tuple */
    >
    constexpr decltype(auto) _ToStdTuple_Impl( 
        Tuple<_Types...>& tpl /* Tuple to convert into std::tuple */,
        std::index_sequence<_Idxs...> /* indices */
    )
    {
        return std::make_tuple( _get_Impl<_Idxs>( tpl )... );
    }

    //
    // const lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t... _Idxs, typename... _Types>
    constexpr decltype(auto) _ToStdTuple_Impl( const Tuple<_Types...>& tpl )
    {
        return std::make_tuple( _get_Impl<_Idxs>( tpl )... );
    }

    //
    // volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t... _Idxs, typename... _Types>
    constexpr decltype(auto) _ToStdTuple_Impl( volatile Tuple<_Types...>& tpl )
    {
        return std::make_tuple( _get_Impl<_Idxs>( tpl )... );
    }

    //
    // const volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t... _Idxs, typename... _Types>
    constexpr decltype(auto) _ToStdTuple_Impl( const volatile Tuple<_Types...>& tpl )
    {
        return std::make_tuple( _get_Impl<_Idxs>( tpl )... );
    }

    //
    // rvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t... _Idxs, typename... _Types>
    constexpr decltype(auto) _ToStdTuple_Impl( Tuple<_Types...>&& tpl )
    {
        return std::make_tuple( 
            _get_Impl<_Idxs>( std::forward<Tuple<_Types...>>( tpl ) )... 
        );
    }

} // anonymous namespace

    /************************************************************************************/

    //
    // Tuple that you can use in application
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

        constexpr size_t Size() { return sizeof...( _Types ); }
    };

    /************************************************************************************/

    //
    // 'get' template that you can use in app
    // 

    //
    // lvalue reference
    // 
    template<
        size_t      _Idx   /* Index to retreive value at */, 
        typename... _Types /* Types stored in tuple */
    >
    constexpr decltype(auto) get( Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    //
    // const lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( const Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    //
    // volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( volatile Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    //
    // const volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( const volatile Tuple<_Types...>& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( tpl );
    }

    //
    // rvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx, typename... _Types>
    constexpr decltype(auto) get( Tuple<_Types...>&& tpl ) noexcept
    {
        static_assert( _Idx < Tuple<_Types...>::size, "Out of range in " __FUNCTION__ );
        return _get_Impl<_Idx>( 
            std::forward<Tuple<_Types...>>( tpl ) 
        );
    }

    /************************************************************************************/

    //
    // 'for_each' for tuple that you can use.
    // There are several implementations again.
    // 
    
    //
    // lvalue reference
    // 
    template<
        size_t   _Idx = 0 /* Index of current element in tuple */,
        typename _Func    /* Type of generic function */, 
        typename _Tuple   /* Types stored in tuple */
    > 
    /* This instantiation will be called in case of first invalid
     * index. After that no other instantiations will be compiled. */
    constexpr typename std::enable_if<_Idx == _Tuple::size>::type
    for_each( _Tuple& /* tpl */, _Func&& /* fn */ ) noexcept
    { 
        /* Empty one. Just placeholder. */ 
    }

    template<
        size_t   _Idx = 0 /* Index of current element in tuple */,
        typename _Func    /* Type of generic function */, 
        typename _Tuple   /* Types stored in tuple */
    > 
    /* This instantiation will be called in case of valid indexes. */
    constexpr typename std::enable_if<_Idx < _Tuple::size>::type 
    for_each( _Tuple& tpl, _Func&& fn )
    {
        //
        // Call function with current element and recursively
        // invoke 'for_each' with incremented index.
        // 
        fn( get<_Idx>( tpl ) );
        for_each<_Idx + 1>( tpl, std::forward<_Func>( fn ) );
    }
    
    //
    // const lvalue reference.
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx == _Tuple::size>::type
    for_each( const _Tuple& /* tpl */, _Func&& /* fn */ ) noexcept
    { 
        /* Empty one. Just placeholder. */ 
    }

    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx < _Tuple::size>::type 
    for_each( const _Tuple& tpl, _Func&& fn )
    {
        fn( get<_Idx>( tpl ) );
        for_each<_Idx + 1>( tpl, std::forward<_Func>( fn ) );
    }
    
    //
    // volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx == _Tuple::size>::type
    for_each( volatile _Tuple& /* tpl */, _Func&& /* fn */ ) noexcept
    { 
        /* Empty one. Just placeholder. */ 
    }

    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx < _Tuple::size>::type 
    for_each( volatile _Tuple& tpl, _Func&& fn )
    {
        fn( get<_Idx>( tpl ) );
        for_each<_Idx + 1>( tpl, std::forward<_Func>( fn ) );
    }
    
    //
    // const volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx == _Tuple::size>::type
    for_each( const volatile _Tuple& /* tpl */, _Func&& /* fn */ ) noexcept
    { 
        /* Empty one. Just placeholder. */ 
    }

    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx < _Tuple::size>::type 
    for_each( const volatile _Tuple& tpl, _Func&& fn )
    {
        fn( get<_Idx>( tpl ) );
        for_each<_Idx + 1>( tpl, std::forward<_Func>( fn ) );
    }
    
    //
    // rvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx == _Tuple::size>::type
    for_each( _Tuple&& /* tpl */, _Func&& /* fn */ ) noexcept
    { 
        /* Empty one. Just placeholder. */ 
    }

    template<size_t _Idx = 0, typename _Func, typename _Tuple>
    constexpr typename std::enable_if<_Idx < _Tuple::size>::type 
    for_each( _Tuple&& tpl, _Func&& fn )
    {
        fn( get<_Idx>( std::forward<_Tuple>( tpl ) ) );
        for_each<_Idx + 1>( std::forward<_Tuple>( tpl ), std::forward<_Func>( fn ) );
    }

    /************************************************************************************/

    //
    // ToStdTuple function, that converts Tuple to std::tuple
    // 

    //
    // lvalue reference
    // 
    template<
        typename... _Types /* Types stored in tuple */
    >
    constexpr decltype(auto) ToStdTuple( 
        Tuple<_Types...>& tpl /* Tuple to convert into std::tuple */
    )
    {
        return _ToStdTuple_Impl( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

    //
    // const lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<typename... _Types>
    constexpr decltype(auto) ToStdTuple( const Tuple<_Types...>& tpl )
    {
        return _ToStdTuple_Impl( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

    //
    // volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<typename... _Types>
    constexpr decltype(auto) ToStdTuple( volatile Tuple<_Types...>& tpl )
    {
        return _ToStdTuple_Impl( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

    //
    // const volatile lvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<typename... _Types>
    constexpr decltype(auto) ToStdTuple( const volatile Tuple<_Types...>& tpl )
    {
        return _ToStdTuple_Impl( 
            tpl, std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

    //
    // rvalue reference
    // For some description see 'lvalue reference' above.
    // 
    template<typename... _Types>
    constexpr decltype(auto) ToStdTuple( Tuple<_Types...>&& tpl )
    {
        return _ToStdTuple_Impl( 
            std::forward<Tuple<_Types...>>( tpl ), 
            std::make_index_sequence<sizeof...( _Types )>{} 
        );
    }

} // types
