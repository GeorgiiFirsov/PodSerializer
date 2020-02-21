#pragma once

#include "pch.h"

#include "Traits.h"

/************************************************************************************
 * Supported types.
 * 
 * Requirements to types that are going to be reflected:
 * - Type must not be a reference.
 * - Type must be copy-constructible OR move-constructible AND move-assignable.
 * - Type must not be polymorphic, because of virtual table pointer.
 * - Type must be POD.
 * - Type must not be fundamental.
 * - Type must have standard layout.
 * - Type must not be empty
 *
 ************************************************************************************/

template<typename _Type>
using is_supported_type = \
    traits::conjunction<
        traits::negation<
            std::is_reference<_Type>
        >,
        traits::disjunction<
            std::is_copy_constructible<_Type>,
            traits::conjunction<
                std::is_move_constructible<_Type>,
                std::is_move_assignable<_Type>
            >
        >,
        traits::negation<
            std::is_polymorphic<_Type>
        >,
        std::is_pod<_Type>,
        traits::negation<
            std::is_fundamental<_Type>
        >,
        std::is_standard_layout<_Type>,
        traits::negation<
            std::is_empty<_Type>
        >,
        MSVC_IS_AGGREGATE( _Type )
    >;


//
// Macro to check type requirements.
// 
#define REFLECTION_CHECK_TYPE( __Type )                                                              \
    static_assert(                                                                                   \
        is_supported_type<__Type>::value,                                                            \
        #__Type " in " __FUNCTION__ " doesn't match the requirements for reflection (see Support.h)" \
    );
