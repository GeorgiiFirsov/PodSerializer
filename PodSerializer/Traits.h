#pragma once

#include "pch.h"

#define TRAIT_REGISTER_TYPE( _Type ) \
    template<> struct is_registered_type<_Type> : std::true_type { }

namespace traits {

    /************************************************************************************/

    //
    // std::conjunction implementation
    // 

    template<typename... /* _BoolTs */> 
    struct conjunction : std::true_type { };

    template<typename _BoolT> 
    struct conjunction<_BoolT> : _BoolT { };

    template<typename _BoolT, typename... _BoolTs>
    struct conjunction<_BoolT, _BoolTs...>
        : std::conditional<bool( _BoolT::value ), conjunction<_BoolTs...>, _BoolT>::type { };

    /************************************************************************************/

    //
    // std::disjunction implementation
    // 

    template<typename... /* _Bools */> 
    struct disjunction : std::false_type { };

    template<class _BoolT> 
    struct disjunction<_BoolT> : _BoolT { };

    template<class _BoolT, class... _BoolTs>
    struct disjunction<_BoolT, _BoolTs...>
        : std::conditional<bool( _BoolT::value ), _BoolT, disjunction<_BoolTs...>>::type { };

    /************************************************************************************/

    //
    // std::negation implementation
    // 

    template<class _BoolT>
    struct negation : std::integral_constant<bool, !bool( _BoolT::value )> { };

    /************************************************************************************/

    //
    // Is type registered?
    // 

    template<typename _Type>
    struct is_registered_type : std::false_type { };

    TRAIT_REGISTER_TYPE( unsigned char        );
    TRAIT_REGISTER_TYPE( unsigned short       );
    TRAIT_REGISTER_TYPE( unsigned int         );
    TRAIT_REGISTER_TYPE( unsigned long        );
    TRAIT_REGISTER_TYPE( unsigned long long   );
    TRAIT_REGISTER_TYPE( signed char          );
    TRAIT_REGISTER_TYPE( short                );
    TRAIT_REGISTER_TYPE( int                  );
    TRAIT_REGISTER_TYPE( long                 );
    TRAIT_REGISTER_TYPE( long long            );
    TRAIT_REGISTER_TYPE( char                 );
    TRAIT_REGISTER_TYPE( wchar_t              );
    TRAIT_REGISTER_TYPE( char16_t             );
    TRAIT_REGISTER_TYPE( char32_t             );
    TRAIT_REGISTER_TYPE( float                );
    TRAIT_REGISTER_TYPE( double               );
    TRAIT_REGISTER_TYPE( long double          );
    TRAIT_REGISTER_TYPE( bool                 );
    TRAIT_REGISTER_TYPE( void*                );
    TRAIT_REGISTER_TYPE( const void*          );
    TRAIT_REGISTER_TYPE( volatile void*       );
    TRAIT_REGISTER_TYPE( const volatile void* );
    TRAIT_REGISTER_TYPE( nullptr_t            );

    /************************************************************************************/

    //
    // Is type a registered type or alias of a registered type?
    // 

    template<typename _Type>
    using is_registered_or_aliased = \
        disjunction<
            is_registered_type<_Type>,
            std::is_enum<_Type>
        >;

    /************************************************************************************/

    //
    // MSVC-specific is_aggregate trait
    // 
    
#if defined(_MSC_VER) && _MSC_VER >= 1916

    template<typename _Type>
    struct is_aggregate 
        : std::integral_constant<
            bool, 
            __is_aggregate(_Type)  // Use compiler intrinsic
        > 
    { };

    template<typename _Type>
    constexpr bool is_aggregate_v = is_aggregate<_Type>::value;

#   define MSVC_IS_AGGREGATE( _Type ) traits::is_aggregate<_Type>
#   define MSVC_IS_AGGREGATE_V( _Type ) traits::is_aggregate_v<_Type>

#else

#   define MSVC_IS_AGGREGATE( _Type ) std::true_type
#   define MSVC_IS_AGGREGATE_V( _Type ) true

#endif

} // traits
