#pragma once

#include "pch.h"

namespace io_manipulators {
namespace io_internal {

    //
    // Separator inserter. For internal use.
    //
    
	static int const sp_fmt_id = std::ios_base::xalloc();

	template<typename _Char>
    struct _SepTag
    {
        explicit _SepTag( _Char ch )
            : m_sep( ch ) { }

        _Char m_sep;
    };

    template<typename _Char>
    _SepTag<_Char> set_separator( _Char _Sep )
    { return _SepTag<_Char>{ _Sep }; }

    template<
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& operator<<(
        std::basic_ostream<_Char, _Traits>& stream, const _SepTag<_Char>& sep
    )
    {
        stream.iword( sp_fmt_id ) = sep.m_sep;
        stream.iword( sp_set_fmt_id) = io_manipulators::io_internal::separator_enabled;
        return stream;
    }

    template<
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& operator>>(
        std::basic_istream<_Char, _Traits>& stream, const _SepTag<_Char>& sep
    )
    {
        stream.iword( sp_fmt_id ) = sep.m_sep;
        stream.iword( sp_set_fmt_id) = io_manipulators::io_internal::separator_enabled;
        return stream;
	}

	/************************************************************************************/

    //
    // Separator detector. For internal use.
    // 

	static int const sp_set_fmt_id = std::ios_base::xalloc();

    enum separator_settings
    {
        separator_enabled = 1
    };

    template<
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_ostream<_Char, _Traits>& operator<<(
        std::basic_ostream<_Char, _Traits>& stream, separator_settings flag
    )
    {
        stream.iword( sp_set_fmt_id ) = flag;
        return stream;
    }

    template<
        typename _Char /* Type of chars used in stream */,
        typename _Traits
            = std::char_traits<_Char>
    > constexpr std::basic_istream<_Char, _Traits>& operator>>(
        std::basic_istream<_Char, _Traits>& stream, separator_settings flag
    )
    {
        stream.iword( sp_set_fmt_id ) = flag;
        return stream;
    }

} // namespace io_internal

    /************************************************************************************/

    //
    // It is useful to be able to control formatting in streams.
    // Following code defines two flags to switch between
    // plain and "beautiful" formatting.
    // 

    static int const bs_fmt_id = std::ios_base::xalloc();

namespace flags {
    enum _Flags
    {
        no_beautiful_struct = 0,
        beautiful_struct = 1
    };
} //namespace flags

} // namespace io_manipulators

template<
    typename _Char /* Type of chars used in stream */,
    typename _Traits
        = std::char_traits<_Char>
> constexpr std::basic_ostream<_Char, _Traits>& operator<<(
    std::basic_ostream<_Char, _Traits>& stream, io_manipulators::flags::_Flags flag
)
{
    stream.iword( io_manipulators::bs_fmt_id ) = flag;
    return stream;
}

template<
    typename _Char /* Type of chars used in stream */,
    typename _Traits
        = std::char_traits<_Char>
> constexpr std::basic_istream<_Char, _Traits>& operator>>(
    std::basic_istream<_Char, _Traits>& stream, io_manipulators::flags::_Flags flag
)
{
    stream.iword( io_manipulators::bs_fmt_id ) = flag;
    return stream;
}
