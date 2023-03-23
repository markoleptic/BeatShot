//
// Copyright 2018 Stefan Seefeld
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_HPP
#define BOOST_GIL_HPP

#include <LocalBoost/boost/gil/algorithm.hpp>
#include <LocalBoost/boost/gil/bit_aligned_pixel_iterator.hpp>
#include <LocalBoost/boost/gil/bit_aligned_pixel_reference.hpp>
#include <LocalBoost/boost/gil/channel.hpp>
#include <LocalBoost/boost/gil/channel_algorithm.hpp>
#include <LocalBoost/boost/gil/cmyk.hpp>
#include <LocalBoost/boost/gil/color_base.hpp>
#include <LocalBoost/boost/gil/color_base_algorithm.hpp>
#include <LocalBoost/boost/gil/color_convert.hpp>
#include <LocalBoost/boost/gil/concepts.hpp>
#include <LocalBoost/boost/gil/device_n.hpp>
#include <LocalBoost/boost/gil/dynamic_step.hpp>
#include <LocalBoost/boost/gil/gray.hpp>
#include <LocalBoost/boost/gil/histogram.hpp>
#include <LocalBoost/boost/gil/image.hpp>
#include <LocalBoost/boost/gil/image_view.hpp>
#include <LocalBoost/boost/gil/image_view_factory.hpp>
#include <LocalBoost/boost/gil/iterator_from_2d.hpp>
#include <LocalBoost/boost/gil/locator.hpp>
#include <LocalBoost/boost/gil/metafunctions.hpp>
#include <LocalBoost/boost/gil/packed_pixel.hpp>
#include <LocalBoost/boost/gil/pixel.hpp>
#include <LocalBoost/boost/gil/pixel_iterator.hpp>
#include <LocalBoost/boost/gil/pixel_iterator_adaptor.hpp>
#include <LocalBoost/boost/gil/planar_pixel_iterator.hpp>
#include <LocalBoost/boost/gil/planar_pixel_reference.hpp>
#include <LocalBoost/boost/gil/point.hpp>
#include <LocalBoost/boost/gil/position_iterator.hpp>
#include <LocalBoost/boost/gil/premultiply.hpp>
#include <LocalBoost/boost/gil/promote_integral.hpp>
#include <LocalBoost/boost/gil/extension/rasterization/circle.hpp>
#include <LocalBoost/boost/gil/extension/rasterization/ellipse.hpp>
#include <LocalBoost/boost/gil/extension/rasterization/line.hpp>
#include <LocalBoost/boost/gil/rgb.hpp>
#include <LocalBoost/boost/gil/rgba.hpp>
#include <LocalBoost/boost/gil/step_iterator.hpp>
#include <LocalBoost/boost/gil/typedefs.hpp>
#include <LocalBoost/boost/gil/utilities.hpp>
#include <LocalBoost/boost/gil/virtual_locator.hpp>
#include <LocalBoost/boost/gil/image_processing/adaptive_histogram_equalization.hpp>
#include "LocalBoost/boost/gil/extension/image_processing/diffusion.hpp"
#include <LocalBoost/boost/gil/image_processing/filter.hpp>
#include <LocalBoost/boost/gil/image_processing/harris.hpp>
#include <LocalBoost/boost/gil/image_processing/hessian.hpp>
#include <LocalBoost/boost/gil/image_processing/histogram_equalization.hpp>
#include <LocalBoost/boost/gil/image_processing/histogram_matching.hpp>
#include "LocalBoost/boost/gil/extension/image_processing/hough_parameter.hpp"
#include "LocalBoost/boost/gil/extension/image_processing/hough_transform.hpp"
#include <LocalBoost/boost/gil/image_processing/morphology.hpp>
#include <LocalBoost/boost/gil/image_processing/numeric.hpp>
#include <LocalBoost/boost/gil/image_processing/scaling.hpp>
#include <LocalBoost/boost/gil/image_processing/threshold.hpp>

#endif
