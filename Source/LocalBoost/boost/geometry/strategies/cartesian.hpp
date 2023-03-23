// Boost.Geometry

// Copyright (c) 2020-2021, Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_STRATEGIES_CARTESIAN_HPP
#define BOOST_GEOMETRY_STRATEGIES_CARTESIAN_HPP


#include <LocalBoost/boost/geometry/strategies/area/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/azimuth/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/buffer/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/convex_hull/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/distance/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/envelope/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/expand/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/io/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/index/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/is_convex/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/relate/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/simplify/cartesian.hpp>


namespace boost { namespace geometry
{

    
namespace strategies
{


template <typename CalculationType = void>
class cartesian
    // derived from the umbrella strategy defining the most strategies
    : public strategies::index::cartesian<CalculationType>
{
public:

    static auto azimuth()
    {
        return strategy::azimuth::cartesian<CalculationType>();
    }

    static auto point_order()
    {
        return strategy::point_order::cartesian<CalculationType>();
    }
};


} // namespace strategies


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_STRATEGIES_CARTESIAN_HPP
