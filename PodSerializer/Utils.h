#pragma once

#include "pch.h"

#include "SizeTArray.h"
#include "Support.h"

#define use_as_return_type( __Type ) __Type{}

//
// Easiest way to assign an id to a type
// 
#define REFLECTION_REGISTER_TYPE( _Type, _Integer )                                                          \
    constexpr types::SizeTArray<1> _GetIdByType( utils::IdenticalType<_Type> ) noexcept { return { { _Integer } }; } \
    constexpr _Type  _GetTypeById( utils::SizeT<_Integer> ) noexcept { constexpr _Type result{}; return result; }

//
// Macro that produces compilation error with specified message
// 
#define ENFORCE_COMPILER_ERROR( _Message ) static_assert( false, #_Message )

namespace reflection {
namespace utils {

    /************************************************************************************/

    //
    // Forward declaration
    // 
    template<size_t /* _Idx */> 
    struct _IndexedUniversalInit;

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

    template<
        typename  _Type /* Type to convert into raw types ids array */,
        size_t... _Idxs /* Indices of internal types */
    > constexpr auto _GetIdsRaw_Impl( std::index_sequence<_Idxs...> )
        noexcept( std::is_nothrow_constructible<_Type, _UniversalInit<_Idxs>...>::value )
    {
        using types::SizeTArray;

        //
        // Each id will be written at index equal to sum of sizes
        // of all previous types in struct. Actual indices will
        // be stored in 'offsets'
        // 
        constexpr SizeTArray<sizeof( _Type )> idsRaw{ { 0 } };
        constexpr SizeTArray<sizeof...( _Idxs )> offsets{ { 0 } };
        constexpr SizeTArray<sizeof...( _Idxs )> sizes /* dummy */ { { 0 } }; 

        //
        // Write offsets of each id into array by creating temporary object.
        // 
        constexpr _Type temporary1{
            _OffsetsUniversalInit<_Idxs, sizeof...( _Idxs )>{ 
                const_cast<size_t*>( offsets.data ), 
                const_cast<size_t*>( sizes.data ) 
            }...
        };

        //
        // Here we write ids into array by creating temporary object.
        // 
        constexpr _Type temporary2{ 
            _IndexedUniversalInit<_Idxs>{ const_cast<size_t*>( idsRaw.data + offsets.data[_Idxs] ) }... 
        };

        return idsRaw;
    }

    /************************************************************************************/

    template<typename _Type> 
    constexpr types::SizeTArray<1>
    _GetIdByType( IdenticalType<_Type>, typename std::enable_if<std::is_enum<_Type>::value>::type = 0 ) noexcept;

    template<typename _Type>
    constexpr types::SizeTArray<sizeof( _Type )> 
    _GetIdByType( IdenticalType<_Type>/*, typename std::enable_if<is_supported_type<_Type>::value>::type = 0*/ ) noexcept;

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
    constexpr types::SizeTArray<1>
    _GetIdByType( IdenticalType<_Type>, typename std::enable_if<std::is_enum<_Type>::value>::type ) noexcept
    {
        //
        // If our structure contains enumeration, just extract an underlying type
        // and assume it as type of a field.
        // 
        return _GetIdByType( 
            utils::IdenticalType<typename std::underlying_type<_Type>::type> 
        );
    }

    //
    // This function used to support nested structures. It returns
    // array of identifiers recursively. Then this array and external
    // one will be merged
    // 
    template<typename _Type>
    constexpr types::SizeTArray<sizeof( _Type )> 
    _GetIdByType( IdenticalType<_Type>/*, typename std::enable_if<is_supported_type<_Type>::value>::type*/ ) noexcept
    {
        return _GetIdsRaw_Impl<_Type>( 
            std::make_index_sequence<sizeof( _Type )>{}
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
        size_t /* _Idx */ /* Formal index that makes easier work with variadics */
    > struct _IndexedUniversalInit
    {
        //
        // Pointer to an array of ids of types
        // 
        size_t* pTypeIds;

        template<size_t _IdsCount> 
        constexpr void Assign( const types::SizeTArray<_IdsCount>& ids ) const noexcept
        {
            for (size_t i = 0; i < _IdsCount; ++i) 
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
            Assign( _GetIdByType( IdenticalType<_Type>{} ) );
            return _Type{}; 
        }
    };

    /************************************************************************************/

    //
    // This structure used to write array of offsets
    // of types in structure.
    // 
    template<
        size_t _Idx  /* Index of type inside of structure */,
        size_t _Size /* Size of array of offsets (actually, an amount of fields in structure) */
    > struct _OffsetsUniversalInit
    {
        size_t* pOffsets; // Pointer to the beginning of offsets array
        size_t* pSizes;   // Pointer to the beginning of sizes array

        template<
            typename _Type /* Type to be initialized */
        > constexpr operator _Type() const noexcept
        {
            const_cast<size_t*>( pOffsets )[_Idx] = _Idx != 0 ? pOffsets[_Idx - 1] + pSizes[_Idx - 1] : 0;
            const_cast<size_t*>( pSizes )[_Idx] = sizeof(_Type);
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