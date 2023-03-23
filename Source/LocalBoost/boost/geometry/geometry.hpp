// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2015 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2015 Mateusz Loskot, London, UK.

// This file was modified by Oracle on 2014-2021.
// Modifications copyright (c) 2014-2021 Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_GEOMETRY_HPP
#define BOOST_GEOMETRY_GEOMETRY_HPP

#include <LocalBoost/boost/config.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)
#error "Use C++14 or higher to compile Boost.Geometry, or use Boost 1.72 or lower."
#endif

// Shortcut to include all header files

#include <LocalBoost/boost/geometry/core/closure.hpp>
#include <LocalBoost/boost/geometry/core/coordinate_dimension.hpp>
#include <LocalBoost/boost/geometry/core/coordinate_system.hpp>
#include <LocalBoost/boost/geometry/core/coordinate_type.hpp>
#include <LocalBoost/boost/geometry/core/cs.hpp>
#include <LocalBoost/boost/geometry/core/geometry_types.hpp>
#include <LocalBoost/boost/geometry/core/interior_type.hpp>
#include <LocalBoost/boost/geometry/core/point_order.hpp>
#include <LocalBoost/boost/geometry/core/point_type.hpp>
#include <LocalBoost/boost/geometry/core/ring_type.hpp>
#include <LocalBoost/boost/geometry/core/tag.hpp>
#include <LocalBoost/boost/geometry/core/tag_cast.hpp>
#include <LocalBoost/boost/geometry/core/tags.hpp>
#include <LocalBoost/boost/geometry/core/visit.hpp>

// Core algorithms
#include <LocalBoost/boost/geometry/core/access.hpp>
#include <LocalBoost/boost/geometry/core/exterior_ring.hpp>
#include <LocalBoost/boost/geometry/core/interior_rings.hpp>
#include <LocalBoost/boost/geometry/core/radian_access.hpp>
#include <LocalBoost/boost/geometry/core/radius.hpp>
#include <LocalBoost/boost/geometry/core/topological_dimension.hpp>

#include <LocalBoost/boost/geometry/arithmetic/arithmetic.hpp>
#include <LocalBoost/boost/geometry/arithmetic/dot_product.hpp>

#include <LocalBoost/boost/geometry/strategies/strategies.hpp>

#include <LocalBoost/boost/geometry/algorithms/append.hpp>
#include <LocalBoost/boost/geometry/algorithms/area.hpp>
#include <LocalBoost/boost/geometry/algorithms/assign.hpp>
#include <LocalBoost/boost/geometry/algorithms/azimuth.hpp>
#include <LocalBoost/boost/geometry/algorithms/buffer.hpp>
#include <LocalBoost/boost/geometry/algorithms/centroid.hpp>
#include <LocalBoost/boost/geometry/algorithms/clear.hpp>
#include <LocalBoost/boost/geometry/algorithms/closest_points.hpp>
#include <LocalBoost/boost/geometry/algorithms/comparable_distance.hpp>
#include <LocalBoost/boost/geometry/algorithms/convert.hpp>
#include <LocalBoost/boost/geometry/algorithms/convex_hull.hpp>
#include <LocalBoost/boost/geometry/algorithms/correct.hpp>
#include <LocalBoost/boost/geometry/algorithms/covered_by.hpp>
#include <LocalBoost/boost/geometry/algorithms/crosses.hpp>
#include <LocalBoost/boost/geometry/algorithms/densify.hpp>
#include <LocalBoost/boost/geometry/algorithms/difference.hpp>
#include <LocalBoost/boost/geometry/algorithms/discrete_frechet_distance.hpp>
#include <LocalBoost/boost/geometry/algorithms/discrete_hausdorff_distance.hpp>
#include <LocalBoost/boost/geometry/algorithms/disjoint.hpp>
#include <LocalBoost/boost/geometry/algorithms/distance.hpp>
#include <LocalBoost/boost/geometry/algorithms/envelope.hpp>
#include <LocalBoost/boost/geometry/algorithms/equals.hpp>
#include <LocalBoost/boost/geometry/algorithms/expand.hpp>
#include <LocalBoost/boost/geometry/algorithms/for_each.hpp>
#include <LocalBoost/boost/geometry/algorithms/intersection.hpp>
#include <LocalBoost/boost/geometry/algorithms/intersects.hpp>
#include <LocalBoost/boost/geometry/algorithms/is_empty.hpp>
#include <LocalBoost/boost/geometry/algorithms/is_simple.hpp>
#include <LocalBoost/boost/geometry/algorithms/is_valid.hpp>
#include <LocalBoost/boost/geometry/algorithms/length.hpp>
#include <LocalBoost/boost/geometry/algorithms/line_interpolate.hpp>
#include <LocalBoost/boost/geometry/algorithms/make.hpp>
#include <LocalBoost/boost/geometry/algorithms/num_geometries.hpp>
#include <LocalBoost/boost/geometry/algorithms/num_interior_rings.hpp>
#include <LocalBoost/boost/geometry/algorithms/num_points.hpp>
#include <LocalBoost/boost/geometry/algorithms/num_segments.hpp>
#include <LocalBoost/boost/geometry/algorithms/overlaps.hpp>
#include <LocalBoost/boost/geometry/algorithms/perimeter.hpp>
#include <LocalBoost/boost/geometry/algorithms/relate.hpp>
#include <LocalBoost/boost/geometry/algorithms/relation.hpp>
#include <LocalBoost/boost/geometry/algorithms/remove_spikes.hpp>
#include <LocalBoost/boost/geometry/algorithms/reverse.hpp>
#include <LocalBoost/boost/geometry/algorithms/simplify.hpp>
#include <LocalBoost/boost/geometry/algorithms/sym_difference.hpp>
#include <LocalBoost/boost/geometry/algorithms/touches.hpp>
#include <LocalBoost/boost/geometry/algorithms/transform.hpp>
#include <LocalBoost/boost/geometry/algorithms/union.hpp>
#include <LocalBoost/boost/geometry/algorithms/unique.hpp>
#include <LocalBoost/boost/geometry/algorithms/within.hpp>

// check includes all concepts
#include <LocalBoost/boost/geometry/geometries/concepts/check.hpp>

#include <LocalBoost/boost/geometry/srs/srs.hpp>

#include <LocalBoost/boost/geometry/util/for_each_coordinate.hpp>
#include <LocalBoost/boost/geometry/util/math.hpp>
#include <LocalBoost/boost/geometry/util/select_coordinate_type.hpp>
#include <LocalBoost/boost/geometry/util/select_most_precise.hpp>

#include <LocalBoost/boost/geometry/views/box_view.hpp>
#include <LocalBoost/boost/geometry/views/closeable_view.hpp>
#include <LocalBoost/boost/geometry/views/identity_view.hpp>
#include <LocalBoost/boost/geometry/views/reversible_view.hpp>
#include <LocalBoost/boost/geometry/views/segment_view.hpp>

#include <LocalBoost/boost/geometry/io/io.hpp>
#include <LocalBoost/boost/geometry/io/dsv/write.hpp>
#include <LocalBoost/boost/geometry/io/svg/svg_mapper.hpp>
#include <LocalBoost/boost/geometry/io/svg/write.hpp>
#include <LocalBoost/boost/geometry/io/wkt/read.hpp>
#include <LocalBoost/boost/geometry/io/wkt/write.hpp>

#endif // BOOST_GEOMETRY_GEOMETRY_HPP
