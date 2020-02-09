#pragma once

#include "pch.h"

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
 *
 ************************************************************************************/

template<typename _Type>
constexpr bool is_supported_type = \
    std::conjunction<
        std::negation<
            std::is_reference<_Type>
        >,
        std::disjunction<
            std::is_copy_constructible<_Type>,
            std::conjunction<
                std::is_move_constructible<_Type>,
                std::is_move_assignable<_Type>
            >
        >,
        std::negation<
            std::is_polymorphic<_Type>
        >,
        std::is_pod<_Type>,
        std::negation<
            std::is_fundamental<_Type>
        >,
        std::is_standard_layout<_Type>
    >::value;


//
// Macro to check type requirements.
// 
#define REFLECTION_CHECK_TYPE( __Type )                                                              \
    static_assert(                                                                                   \
        is_supported_type<##__Type>,                                                                 \
        #__Type " in " __FUNCTION__ " doesn't match the requirements for reflection (see Support.h)" \
    );