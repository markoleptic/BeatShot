// Boost.Units - A C++ library for zero-overhead dimensional analysis and 
// unit/quantity manipulation and conversion
//
// Copyright (C) 2003-2008 Matthias Christian Schabel
// Copyright (C) 2008 Steven Watanabe
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNITS_ENERGY_DERIVED_DIMENSION_HPP
#define BOOST_UNITS_ENERGY_DERIVED_DIMENSION_HPP

#include <LocalBoost/boost/units/derived_dimension.hpp>
#include <LocalBoost/boost/units/physical_dimensions/length.hpp>
#include <LocalBoost/boost/units/physical_dimensions/mass.hpp>
#include <LocalBoost/boost/units/physical_dimensions/time.hpp>

namespace boost {

namespace units {

/// derived dimension for energy : L^2 M T^-2
typedef derived_dimension<length_base_dimension,2,
                          mass_base_dimension,1,
                          time_base_dimension,-2>::type energy_dimension;                

} // namespace units

} // namespace boost

#endif // BOOST_UNITS_ENERGY_DERIVED_DIMENSION_HPP