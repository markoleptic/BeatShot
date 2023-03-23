/*
 * Copyright 2016 Andrey Semashev
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef BOOST_WINAPI_ACCESS_RIGHTS_HPP_INCLUDED_
#define BOOST_WINAPI_ACCESS_RIGHTS_HPP_INCLUDED_

#include <LocalBoost/boost/winapi/basic_types.hpp>
#include <LocalBoost/boost/winapi/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace winapi {

#if defined( BOOST_USE_WINDOWS_H )

BOOST_CONSTEXPR_OR_CONST DWORD_ DELETE_ = DELETE;
BOOST_CONSTEXPR_OR_CONST DWORD_ READ_CONTROL_ = READ_CONTROL;
BOOST_CONSTEXPR_OR_CONST DWORD_ WRITE_DAC_ = WRITE_DAC;
BOOST_CONSTEXPR_OR_CONST DWORD_ WRITE_OWNER_ = WRITE_OWNER;
BOOST_CONSTEXPR_OR_CONST DWORD_ SYNCHRONIZE_ = SYNCHRONIZE;

BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_ALL_ = STANDARD_RIGHTS_ALL;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_EXECUTE_ = STANDARD_RIGHTS_EXECUTE;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_READ_ = STANDARD_RIGHTS_READ;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_REQUIRED_ = STANDARD_RIGHTS_REQUIRED;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_WRITE_ = STANDARD_RIGHTS_WRITE;

BOOST_CONSTEXPR_OR_CONST DWORD_ SPECIFIC_RIGHTS_ALL_ = SPECIFIC_RIGHTS_ALL;

BOOST_CONSTEXPR_OR_CONST DWORD_ ACCESS_SYSTEM_SECURITY_ = ACCESS_SYSTEM_SECURITY;

BOOST_CONSTEXPR_OR_CONST DWORD_ MAXIMUM_ALLOWED_ = MAXIMUM_ALLOWED;

BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_ALL_ = GENERIC_ALL;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_EXECUTE_ = GENERIC_EXECUTE;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_WRITE_ = GENERIC_WRITE;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_READ_ = GENERIC_READ;

typedef ::ACCESS_MASK ACCESS_MASK_;
typedef ::PACCESS_MASK PACCESS_MASK_;

#else // defined( BOOST_USE_WINDOWS_H )

BOOST_CONSTEXPR_OR_CONST DWORD_ DELETE_ = 0x00010000;
BOOST_CONSTEXPR_OR_CONST DWORD_ READ_CONTROL_ = 0x00020000;
BOOST_CONSTEXPR_OR_CONST DWORD_ WRITE_DAC_ = 0x00040000;
BOOST_CONSTEXPR_OR_CONST DWORD_ WRITE_OWNER_ = 0x00080000;
BOOST_CONSTEXPR_OR_CONST DWORD_ SYNCHRONIZE_ = 0x00100000;

BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_ALL_ = 0x001F0000;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_EXECUTE_ = READ_CONTROL_;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_READ_ = READ_CONTROL_;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_REQUIRED_ = 0x000F0000;
BOOST_CONSTEXPR_OR_CONST DWORD_ STANDARD_RIGHTS_WRITE_ = READ_CONTROL_;

BOOST_CONSTEXPR_OR_CONST DWORD_ SPECIFIC_RIGHTS_ALL_ = 0x0000FFFF;

BOOST_CONSTEXPR_OR_CONST DWORD_ ACCESS_SYSTEM_SECURITY_ = 0x01000000;

BOOST_CONSTEXPR_OR_CONST DWORD_ MAXIMUM_ALLOWED_ = 0x02000000;

BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_ALL_ = 0x10000000;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_EXECUTE_ = 0x20000000;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_WRITE_ = 0x40000000;
BOOST_CONSTEXPR_OR_CONST DWORD_ GENERIC_READ_ = 0x80000000;

typedef DWORD_ ACCESS_MASK_;
typedef ACCESS_MASK_* PACCESS_MASK_;

#endif // defined( BOOST_USE_WINDOWS_H )

}
}

#include <LocalBoost/boost/winapi/detail/footer.hpp>

#endif // BOOST_WINAPI_ACCESS_RIGHTS_HPP_INCLUDED_
