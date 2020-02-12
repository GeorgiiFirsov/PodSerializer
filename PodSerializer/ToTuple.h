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

	template<
		typename _Type
	> constexpr size_t GetTotalFieldsCount() noexcept
	{
		constexpr auto ids = GetTypeIds<_Type>();
		return ids.size();
	}

    /************************************************************************************/

    template<
        typename  _Type /* Type to convert into a tuple */,
        size_t... _Idxs /* Indices of internal types (with expanded nested structures) */
    > constexpr auto _ToTuple_Impl( 
        _Type& obj /* Object to convert into a structure */, 
        std::index_sequence<_Idxs...> /* indices */ 
    ) noexcept
    {
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

		tuple_t tpl;
		size_t offset = 0;

		auto PutAligned = [src = reinterpret_cast<const char*>( &obj ), &offset]( auto& element ) 
		{
			using element_t = typename std::decay<decltype( element )>::type;

			while (offset % alignof( element_t ) != 0) offset++;
			
			element = *reinterpret_cast<const element_t*>( src + offset );
			offset += sizeof( element_t );
		};

		types::for_each( tpl, PutAligned );

		return tpl;

        //
        // Bad black magic }-)
        // Just trust me here - tuple and passed structure have the same layout,
        // but can not be casted to each other directly.
        // 
        auto pObj = static_cast<const void*>( &obj );
        return *static_cast<const tuple_t*>( pObj );
    }

} // details

                             /* ^^^  Library internals  ^^^ */
    /************************************************************************************/
                             /* vvv       User API      vvv */

    template<typename _Type> 
    constexpr /* Maybe it is possible to construct our tuple in compile-time */
    decltype(auto) ToTuple( 
         const _Type& obj /* Object to convert into tuple */
    ) noexcept
    {
        using _CleanType = typename std::remove_cv<_Type>::type;

        REFLECTION_CHECK_TYPE( _CleanType );

        return details::_ToTuple_Impl( 
            obj, std::make_index_sequence<details::GetTotalFieldsCount<_Type>()>{} 
        );
    }

} // reflection