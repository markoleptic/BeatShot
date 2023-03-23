//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/json
//

#ifndef BOOST_JSON_SRC_HPP
#define BOOST_JSON_SRC_HPP

/*

This file is meant to be included once,
in a translation unit of the program.

*/

#ifndef BOOST_JSON_SOURCE
#define BOOST_JSON_SOURCE
#endif

// We include this in case someone is using
// src.hpp as their main JSON header file
// https://github.com/boostorg/json/issues/223#issuecomment-689264149
#include <LocalBoost/boost/json.hpp>

#include <LocalBoost/boost/json/detail/config.hpp>

#include <LocalBoost/boost/json/impl/array.ipp>
#include <LocalBoost/boost/json/impl/error.ipp>
#include <LocalBoost/boost/json/impl/kind.ipp>
#include <LocalBoost/boost/json/impl/monotonic_resource.ipp>
#include <LocalBoost/boost/json/impl/null_resource.ipp>
#include <LocalBoost/boost/json/impl/object.ipp>
#include <LocalBoost/boost/json/impl/parse.ipp>
#include <LocalBoost/boost/json/impl/parser.ipp>
#include <LocalBoost/boost/json/impl/pointer.ipp>
#include <LocalBoost/boost/json/impl/serialize.ipp>
#include <LocalBoost/boost/json/impl/serializer.ipp>
#include <LocalBoost/boost/json/impl/static_resource.ipp>
#include <LocalBoost/boost/json/impl/stream_parser.ipp>
#include <LocalBoost/boost/json/impl/string.ipp>
#include <LocalBoost/boost/json/impl/value.ipp>
#include <LocalBoost/boost/json/impl/value_stack.ipp>
#include <LocalBoost/boost/json/impl/value_ref.ipp>

#include <LocalBoost/boost/json/detail/impl/shared_resource.ipp>
#include <LocalBoost/boost/json/detail/impl/default_resource.ipp>
#include <LocalBoost/boost/json/detail/impl/except.ipp>
#include <LocalBoost/boost/json/detail/impl/format.ipp>
#include <LocalBoost/boost/json/detail/impl/handler.ipp>
#include <LocalBoost/boost/json/detail/impl/stack.ipp>
#include <LocalBoost/boost/json/detail/impl/string_impl.ipp>

#include <LocalBoost/boost/json/detail/ryu/impl/d2s.ipp>

#endif
