#pragma once

#include "pch.h"

#define use_as_return_type( __Type ) __Type{}

//
// Type passed to functions in reflection functions should be pod, but not fundamental
// 
#define REFLECTION_CHECK_TYPE( __Type )                                                                              \
    static_assert(                                                                                                   \
        std::is_pod<##__Type>::value, #__Type " in " __FUNCTION__ " should be POD"                                   \
    );                                                                                                               \
    static_assert(                                                                                                   \
        std::negation<std::is_fundamental<##__Type>>::value, #__Type " in " __FUNCTION__ " shouldn't be fundamental" \
    );

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
    // Structure used to "initialize" any variable of any type.
    // 
    template<
        size_t /* _Idx */ /* Formal index that makes easier work with variadics */
    > struct _UniversalInit
    {
        template<
            typename _Type /* Type to be initialized */
        > constexpr operator _Type&() const noexcept;
    };

    /************************************************************************************/

    //
    // Fundamental types registration
    // 

    REFLECTION_REGISTER_TYPE( unsigned char       , 1  );
    REFLECTION_REGISTER_TYPE( unsigned short      , 2  );
    REFLECTION_REGISTER_TYPE( unsigned int        , 3  );
    REFLECTION_REGISTER_TYPE( unsigned long       , 4  );
    REFLECTION_REGISTER_TYPE( unsigned long long  , 5  );
    REFLECTION_REGISTER_TYPE( signed char         , 6  );
    REFLECTION_REGISTER_TYPE( short               , 7  );
    REFLECTION_REGISTER_TYPE( int                 , 8  );
    REFLECTION_REGISTER_TYPE( long                , 9  );
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
            const_cast<size_t*>( pTypeIds )[_Idx] = _GetIdByType( IdenticalType<_Type>{} );
            return _Type{}; 
        }
    };

    /************************************************************************************/

    //
    // Class that represents an array with compile-time features
    // 
    template<
        size_t _Size
    > class SizeTArray final
    {
    public:
        size_t data[_Size];
        static constexpr size_t size() noexcept { return _Size; }
    };

    template<>
    class SizeTArray<0> final
    {
    public:
        size_t data[1]; /* Zero sized arrays are not allowed */
        static constexpr size_t size() noexcept { return 0; }
    };

    #define array_data( _Array ) _Array.data

    //
    // Indexed access to compile-time array
    // 
    template<
        size_t _Idx,
        size_t _Size
    > constexpr size_t get( const SizeTArray<_Size>& array )
    {
        static_assert( _Idx < _Size, "Out of range in " __FUNCTION__ );
        return array.data[_Idx];
    }

} // utils
} // refletion