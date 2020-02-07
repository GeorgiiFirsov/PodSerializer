#pragma once

#include "pch.h"

namespace types {

    /************************************************************************************/

    //
    // Class that represents an array with compile-time features
    // 
    template<size_t _Size> 
    class SizeTArray final
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

    /************************************************************************************/

    //
    // Indexed access to compile-time array
    // 
    template<
        size_t _Idx  /* Index of element to get */,
        size_t _Size /* Size of array */
    > constexpr size_t get( const SizeTArray<_Size>& array )
    {
        static_assert( _Idx < _Size, "Out of range in " __FUNCTION__ );
        return array.data[_Idx];
    }

}