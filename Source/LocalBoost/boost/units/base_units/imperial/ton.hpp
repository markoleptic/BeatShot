// Boost.Units - A C++ library for zero-overhead dimensional analysis and 
// unit/quantity manipulation and conversion
//
// Copyright (C) 2003-2008 Matthias Christian Schabel
// Copyright (C) 2007-2008 Steven Watanabe
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNIT_SYSTEMS_IMPERIAL_TON_HPP_INCLUDED
#define BOOST_UNIT_SYSTEMS_IMPERIAL_TON_HPP_INCLUDED

#include <LocalBoost/boost/units/scaled_base_unit.hpp>
#include <LocalBoost/boost/units/static_rational.hpp>
#include <LocalBoost/boost/units/scale.hpp>
#include <LocalBoost/boost/units/base_units/imperial/pound.hpp>

namespace boost {

namespace units {

namespace imperial {

typedef scaled_base_unit<pound_base_unit, scale<2240, static_rational<1> > > ton_base_unit;

} // namespace imperial

template<>
struct base_unit_info<imperial::ton_base_unit> {
    static BOOST_CONSTEXPR const char* name()   { return("long ton"); }
    static BOOST_CONSTEXPR const char* symbol() { return("t"); }
};

} // namespace units

} // namespace boost

#endif // BOOST_UNIT_SYSTEMS_IMPERIAL_TON_HPP_INCLUDED
