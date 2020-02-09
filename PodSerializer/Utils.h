#pragma once

#include "pch.h"

#define use_as_return_type( __Type ) __Type{}

//
// Easiest way to assign an id to a type
// 
#define REFLECTION_REGISTER_TYPE( _Type, _Integer )                                                   \
    constexpr inline size_t _GetIdByType( utils::IdenticalType<_Type> ) noexcept { return _Integer; } \
    constexpr inline _Type  _GetTypeById( utils::SizeT<_Integer> ) noexcept { constexpr _Type result{}; return result; }

//
// Macro that produces compilation error with specified message
// 
#define ENFORCE_COMPILER_ERROR( _Message ) static_assert( false, #_Message )

namespace reflection {
namespace utils {

    /************************************************************************************/

    //
    // Converts type to itself
    // 
    template<typename _Type>
    struct Identity {
        using type = _Type;
    };

    template<typename _Type>
    using IdenticalType = typename Identity<_Type>::type;

    /************************************************************************************/

    template<size_t _Integer>
    using SizeT = std::integral_constant<size_t, _Integer>;

    /************************************************************************************/

    //
    // Fundamental types registration
    // 

    REFLECTION_REGISTER_TYPE( unsigned char       ,  1 );
    REFLECTION_REGISTER_TYPE( unsigned short      ,  2 );
    REFLECTION_REGISTER_TYPE( unsigned int        ,  3 );
    REFLECTION_REGISTER_TYPE( unsigned long       ,  4 );
    REFLECTION_REGISTER_TYPE( unsigned long long  ,  5 );
    REFLECTION_REGISTER_TYPE( signed char         ,  6 );
    REFLECTION_REGISTER_TYPE( short               ,  7 );
    REFLECTION_REGISTER_TYPE( int                 ,  8 );
    REFLECTION_REGISTER_TYPE( long                ,  9 );
    REFLECTION_REGISTER_TYPE( long long           , 10 );
    REFLECTION_REGISTER_TYPE( char                , 11 );
    REFLECTION_REGISTER_TYPE( wchar_t             , 12 );
    REFLECTION_REGISTER_TYPE( char16_t            , 13 );
    REFLECTION_REGISTER_TYPE( char32_t            , 14 );
    REFLECTION_REGISTER_TYPE( float               , 15 );
    REFLECTION_REGISTER_TYPE( double              , 16 );
    REFLECTION_REGISTER_TYPE( long double         , 17 );
    REFLECTION_REGISTER_TYPE( bool                , 18 );
    REFLECTION_REGISTER_TYPE( void*               , 19 );
    REFLECTION_REGISTER_TYPE( const void*         , 20 );
    REFLECTION_REGISTER_TYPE( volatile void*      , 21 );
    REFLECTION_REGISTER_TYPE( const volatile void*, 22 );
    REFLECTION_REGISTER_TYPE( nullptr_t           , 23 );

    template<typename _Type>
    constexpr inline auto _GetIdByType( utils::IdenticalType<_Type> ) noexcept
        -> typename std::enable_if<std::is_enum<_Type>::value, size_t>::type
    {
        //
        // If our structure contains enumeration, just extract an underlying type
        // and assume it as type of a field.
        // 
        return _GetIdByType( 
            utils::IdenticalType< typename std::underlying_type<_Type>::type> 
        );
    }

    /************************************************************************************/

    //
    // Structure used to "initialize" any variable of any type.
    // 
    template<
        size_t /* _Idx */ /* Formal index that makes easier work with variadics */
    > struct _UniversalInit
    {
        template<
            typename _Type /* Type to be initialized */
        > constexpr operator _Type&() const noexcept; // Not implemented - it's ok :)
    };

    /************************************************************************************/

    //
    // Structure used to cunstruct ids sequence
    // 
    template<
        size_t _Idx /* Index in array of ids for id of current type */
    > struct _IndexedUniversalInit
    {
        //
        // Pointer to an array of ids of types
        // 
        size_t* pTypeIds;

        constexpr void Assign( size_t id ) const noexcept
        {
            //
            // It is necessary to cast constness away, because our
            // calculations are performed in compile-time with constant
            // expressions.
            // 
            const_cast<size_t*>( pTypeIds )[0] = id;
        }

        template<
            typename _TypeId /* Type of container with type ids */
        > 
        constexpr void Assign( const _TypeId& ids ) const noexcept
        {
            static_assert( false, __FUNCTION__ " is not permitted to be used for now" );
            for (size_t i = 0; i < _TypeId::size(); ++i) 
            {
                //
                // It is necessary here to cast constness away. 
                // See explanation above.
                // 
                const_cast<size_t*>( pTypeIds )[i] = ids.data[i];
            }
        }

        template<
            typename _Type /* Type to be initialized and indexed */
        > constexpr operator _Type()
            const noexcept( std::is_nothrow_constructible<_Type>::value )
        {
            //
            // It is necessary to cast constness away, because our
            // calculations are performed in compile-time with constant
            // expressions.
            // 
            constexpr auto ids = _GetIdByType( IdenticalType<_Type>{} );
            Assign( ids );
            return _Type{}; 
        }
    };

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
        constexpr operator _Type()
        {
            //
            // _Stored and _Type could be the same types and 'static_cast' does nothing.
            // But _Stored can be uderlying type of enumeration of type _Type. That is 
            // the reason, why it is necessary to use 'static_cast' here.
            // 
            return static_cast<_Type>( value );
        }
    };

} // utils
} // refletion