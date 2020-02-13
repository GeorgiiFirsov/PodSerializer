#pragma once

#include "pch.h"

#include "Support.h"
#include "SizeTArray.h"
#include "GetFieldsCount.h"
#include "RegisteredTypeTrait.h"


/************************************************************************************
 * GetTypeIds function
 * 
 * The key-concept is following:
 *  - We register all fundamental types explicitly
 *  - Due to registration we have mapping between types and integers
 *  - These functions return compile-time arrays of those integers.
 *    
 ************************************************************************************/


//
// Easiest way to assign an id to a type
// 
#define REFLECTION_REGISTER_TYPE( _Type, _Integer )                                                          \
    constexpr size_t _GetIdByType( IdenticalType<_Type> ) noexcept { return _Integer; }                      \
    constexpr _Type  _GetTypeById( SizeT<_Integer> ) noexcept { constexpr _Type result{}; return result; }


namespace reflection {
namespace details {

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

    //
    // Overload for enumerations
    // 
    template<typename _Type> 
    constexpr typename std::enable_if<std::is_enum<_Type>::value, size_t>::type
    _GetIdByType( IdenticalType<_Type> ) noexcept
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
    // This function used to support nested structures. It returns an
    // array of identifiers recursively. Then this array and external
    // one will be merged
    // 
    template<typename _Type>
    constexpr typename std::enable_if<is_supported_type<_Type>::value, types::SizeTArray<sizeof( _Type )>>::type
    _GetIdsByType( IdenticalType<_Type> ) noexcept
    {
        //
        // See definition of '_GetIdsRaw_Impl' below.
        // 
        return _GetIdsRaw_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{}
        );
    }

    /************************************************************************************/

    //
    // This structure used to write array of offsets
    // of fields in structure.
    // 
    template<
        size_t _Idx  /* Index of type inside of structure */,
        size_t _Size /* Size of array of offsets (actually, a number of fields in structure) */
    > struct _OffsetsUniversalInit
    {
        size_t* pOffsets; // Pointer to the beginning of offsets array
        size_t* pSizes;   // Pointer to the beginning of sizes array

        template<
            typename _Type /* Type to be initialized */
        > constexpr operator _Type() const noexcept
        {
            const_cast<size_t*>( pOffsets )[_Idx] = _Idx != 0 ? pOffsets[_Idx - 1] + pSizes[_Idx - 1] : 0;
            const_cast<size_t*>( pSizes )[_Idx] = sizeof( _Type );
            return _Type{};
        }
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

        //
        // Simply inserts an id into array
        // 
        constexpr void Assign( size_t id ) const noexcept
        {
            const_cast<size_t*>( pTypeIds )[0] = id;
        }

        //
        // Merges an array of ids into the external array
        // 
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

        //
        // Function template called from type conversion
        // operator. It provides the same interface of work
        // around different types of initialized variables.
        // 
        template<typename _Type>
        constexpr typename std::enable_if<is_registered_or_aliased<_Type>::value>::type
        _Cast_Impl() const noexcept
        {
            Assign( _GetIdByType( IdenticalType<_Type>{} ) );
        }
        
        template<typename _Type>
        constexpr typename std::enable_if<!is_registered_or_aliased<_Type>::value>::type
        _Cast_Impl() const noexcept
        {
            Assign( _GetIdsByType<_Type>( IdenticalType<_Type>{} ) );
        }

        template<
            typename _Type /* Type to be initialized and indexed */
        > constexpr operator _Type() const noexcept
        {
            _Cast_Impl<_Type>();
            return _Type{};
        }
    };

    /************************************************************************************/

    template<
        typename  _Type /* Type to convert into raw types ids array */,
        size_t... _Idxs /* Indices of internal types */
    > constexpr auto _GetIdsRaw_Impl( std::index_sequence<_Idxs...> ) noexcept
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

    template<
        typename  _Type  /* Type to return ids for */,
        size_t... _Idxs  /* Indices of internal types */
    > constexpr auto _GetTypeIds_Impl( std::index_sequence<_Idxs...> indices ) noexcept
    {
        using types::ArrayToNonZeros;
        using types::SizeTArray;

        //
        // Raw array with potential zero paddings
        // 
        constexpr auto idsRaw = _GetIdsRaw_Impl<_Type>( indices );

        //
        // Here we need to remove all zeros from 'idsRaw' array.
        // Use helper class ArrayTransformer.
        // 
        constexpr SizeTArray<idsRaw.CountNonZeros()> idsWithoutZeros{ { 0 } };

        constexpr ArrayToNonZeros<sizeof( _Type )> transform{ 
            const_cast<size_t*>( idsRaw.data ), 
            const_cast<size_t*>( idsWithoutZeros.data ) 
        };
        transform.Run();

        return idsWithoutZeros;
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<
        typename _Type /* Type to convert into ids array */
    > constexpr decltype(auto) GetTypeIds() noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetTypeIds_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

    template<
        typename _Type /* Type to convert into ids array */
    > constexpr decltype(auto) GetTypeIds(
        const _Type& /* obj */ /* For implicit template parameter deduction */
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_GetTypeIds_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );
    }

} // reflection