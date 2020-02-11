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

        constexpr size_t CountNonZeros() const noexcept
        {
            size_t result = 0;
            for (size_t i = 0; i < _Size; ++i)
            {
                if (data[i]) {
                    ++result;
                }
            }
            return result;
        }
    };

    template<>
    class SizeTArray<0> final
    {
    public:
        size_t data[1]; /* Zero sized arrays are not allowed */

        static constexpr size_t size() noexcept { return 0; }

        constexpr size_t CountNonZeros() const noexcept { return 0; }
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

    /************************************************************************************/

    //
    // Helper class to write all non-zero values 
    // from first array to second one in compile time.
    // 
    template<
        size_t _Size1    /* Size of first array (source) */,
        size_t _Idx1 = 0 /* Current index in first array (source) */,
        size_t _Idx2 = 0 /* Current index in second array (destination) */
    > struct ArrayTransformer
    {
        size_t* pFirst;  // Pointer to first array
        size_t* pSecond; // Pointer to second array

        //
        // Recursive transformation
        // 
        constexpr void Run() const noexcept
        {
#pragma warning(push)
#pragma warning(disable: 4127) // conditional expression is constant
            if (_Idx1 < _Size1)
            {
                if (pFirst[_Idx1]) 
                {
                    //
                    // Write non-zero value to destination.
                    // 
                    const_cast<size_t*>( pSecond )[_Idx2] = pFirst[_Idx1];

                    //
                    // Instantiate next transformer with incremented indices and run transformation
                    // 
                    ArrayTransformer<_Size1, _Idx1 + 1, _Idx2 + 1> next{ pFirst, pSecond };
                    next.Run();
                }
                else 
                {
                    //
                    // Instantiate next transformer with incremented first index and run transformation
                    // 
                    ArrayTransformer<_Size1, _Idx1 + 1, _Idx2> next{ pFirst, pSecond };
                    next.Run();
                }
            }
#pragma warning(pop)
        }
    };

    //
    // Specialization to break recursive instantiation
    // when we reach end of the first array.
    // 
    template<
        size_t _Size1 /* Size of source */,
        size_t _Idx2  /* Index in destination */
    > struct ArrayTransformer<_Size1, _Size1, _Idx2>
    {
        size_t* pFirst;
        size_t* pSecond;

        constexpr void Run() const noexcept
        { /* Empty */ }
    };

}