///////////////////////////////////////////////////////////////////////////////
// with_error.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ACCUMULATORS_STATISTICS_WITH_ERROR_HPP_EAN_01_11_2005
#define BOOST_ACCUMULATORS_STATISTICS_WITH_ERROR_HPP_EAN_01_11_2005

#include <LocalBoost/boost/preprocessor/repetition/enum_params.hpp>
#include <LocalBoost/boost/mpl/vector.hpp>
#include <LocalBoost/boost/mpl/transform_view.hpp>
#include <LocalBoost/boost/mpl/placeholders.hpp>
#include <LocalBoost/boost/accumulators/statistics_fwd.hpp>
#include <LocalBoost/boost/accumulators/statistics/error_of.hpp>

namespace boost { namespace accumulators
{

namespace detail
{
    template<typename Feature>
    struct error_of_tag
    {
        typedef tag::error_of<Feature> type;
    };
}

///////////////////////////////////////////////////////////////////////////////
// with_error
//
template<BOOST_PP_ENUM_PARAMS(BOOST_ACCUMULATORS_MAX_FEATURES, typename Feature)>
struct with_error
  : mpl::transform_view<
        mpl::vector<BOOST_PP_ENUM_PARAMS(BOOST_ACCUMULATORS_MAX_FEATURES, Feature)>
      , detail::error_of_tag<mpl::_1>
    >
{
};

}} // namespace boost::accumulators

#endif
