#pragma once

#include "pch.h"

#include "Support.h"
#include "Tuple.h"
#include "GetTypeIds.h"
#include "GetFieldsCount.h"


/************************************************************************************
 * ToTuple function
 * 
 * The key-concept is following:
 *  - Convert built inside array of ids of types into types back and use them to 
 *    initialize tuple
 *    
 ************************************************************************************/


namespace reflection {
namespace details {

    /************************************************************************************/

    //
    // Returns total fields number inside a structure of type _Type.
    // It includes fields in all nested structures.
    // 
    template<
        typename _Type /* Type to count all fields in */
    > constexpr size_t GetTotalFieldsCount() noexcept
    {
        constexpr auto ids = GetTypeIds<_Type>();
        return ids.Size();
    }

    /************************************************************************************/

    template<
        typename  _Type /* Type to convert into a tuple */,
        size_t... _Idxs /* Indices of internal types (with expanded nested structures) */
    > constexpr auto _ToTuple_Impl( 
        const _Type& obj /* Object to convert into a structure */, 
        std::index_sequence<_Idxs...> /* indices */ 
    ) noexcept
    {
        using types::ArrayToIndices;
        using types::SizeTArray;
        using types::Tuple;
        using types::get;

        // 
        // Array with ids of types in structure. It is important
        // to make it to be constexpr here.
        // 
        constexpr auto ids = GetTypeIds<_Type>();

        using tuple_t = Tuple<
            /* Walk through all saved ids of tupes and convert back them to types.
             * Until this time it was extremely important to do all our manipulations
             * in compile-time, bacause types, passes to tuple should be known now. */
            decltype( _GetTypeById( SizeT<get<_Idxs>( ids )>{} ) )...
        >;

        //
        // This is the case, when tuple and structure have the same layout.
        // It is possible when they have equal size, because they contain
        // the same types in the same order.
        // Here we can just cast structure to a tuple. ( Black magic }-) )
        // 
        if (sizeof( tuple_t ) == sizeof( _Type )){
            auto pObj = static_cast<const void*>( &obj );
            return *static_cast<const tuple_t*>( pObj );
        }

        //
        // The key-concept of working with nested structs:
        // - They completely break contiguous aligning of data.
        // - We can find out a minimal possible offset of each field
        //   by counting sizes of previous fields.
        // - If this minimal offset match the property offset % alignof( _Type )
        //   it is the place, where our data is stored.
        // - Otherwise we need to find closest largest offset with property above.
        // 

        constexpr auto idsRaw = _GetIdsRaw_Impl<_Type>( 
            std::make_index_sequence<GetFieldsCount<_Type>()>{} 
        );

        //
        // Now we need to calculate possible offsets.
        // Just extract them as indices of non-zero ids.
        // 
        constexpr SizeTArray<idsRaw.CountNonZeros()> offsets{ { 0 } };

        constexpr ArrayToIndices<idsRaw.Size()> transform{
            const_cast<size_t*>( idsRaw.data ),
            const_cast<size_t*>( offsets.data )
        };
        transform.Run();

        //
        // Now it's time to put values into a tuple.
        // we know here their offsets (actually we 
        // can correct them a bit later).
        // 
        tuple_t tpl;
        size_t index = 0;

        auto PutAligned = [src = reinterpret_cast<const char*>( &obj ), &index, &offsets]( auto& element ) 
        {
            using element_t = typename std::decay<decltype( element )>::type;

            size_t offset = offsets.data[index++];
            while (offset % alignof( element_t ) != 0) offset++;

            element = *reinterpret_cast<const element_t*>( src + offset );
        };

        types::for_each( tpl, PutAligned );

        return tpl;
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<typename _Type> 
    constexpr /* Maybe it is possible to construct our tuple in compile-time */
    auto ToTuple( 
         const _Type& obj /* Object to convert into tuple */
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_ToTuple_Impl( 
            obj, std::make_index_sequence<details::GetTotalFieldsCount<_Type>()>{} 
        );
    }

    template<typename _Type>
    constexpr  /* Maybe it is possible to construct our tuple in compile-time */
    auto ToStandardTuple(
         const _Type& obj /* Object to convert into tuple */
    )
    {
        using types::ToStdTuple;

        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        auto tpl = details::_ToTuple_Impl( 
            obj, std::make_index_sequence<details::GetTotalFieldsCount<_Type>()>{} 
        ); 

        return ToStdTuple( tpl );
    }

} // reflection