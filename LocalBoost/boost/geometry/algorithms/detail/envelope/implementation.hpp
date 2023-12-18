// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2015 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2015 Mateusz Loskot, London, UK.

// This file was modified by Oracle on 2015-2021.
// Modifications copyright (c) 2015-2021, Oracle and/or its affiliates.
// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_ENVELOPE_IMPLEMENTATION_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_ENVELOPE_IMPLEMENTATION_HPP

#include <LocalBoost/boost/geometry/core/exterior_ring.hpp>
#include <LocalBoost/boost/geometry/core/interior_rings.hpp>
#include <LocalBoost/boost/geometry/core/tags.hpp>

#include <LocalBoost/boost/geometry/algorithms/is_empty.hpp>

#include <LocalBoost/boost/geometry/algorithms/detail/envelope/areal.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/box.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/geometry_collection.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/linear.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/multipoint.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/point.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/range.hpp>
#include <LocalBoost/boost/geometry/algorithms/detail/envelope/segment.hpp>

#include <LocalBoost/boost/geometry/algorithms/dispatch/envelope.hpp>

#include <LocalBoost/boost/geometry/strategies/envelope/cartesian.hpp>
#include <LocalBoost/boost/geometry/strategies/envelope/geographic.hpp>
#include <LocalBoost/boost/geometry/strategies/envelope/spherical.hpp>

#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_ENVELOPE_IMPLEMENTATION_HPP
