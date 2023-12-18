/*=============================================================================
    Copyright (c) 2014 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_VECTOR20_11052014_2316
#define FUSION_VECTOR20_11052014_2316

#include <LocalBoost/boost/config.hpp>
#include <LocalBoost/boost/fusion/support/config.hpp>
#include <LocalBoost/boost/fusion/container/vector/detail/config.hpp>

///////////////////////////////////////////////////////////////////////////////
// Without variadics, we will use the PP version
///////////////////////////////////////////////////////////////////////////////
#if !defined(BOOST_FUSION_HAS_VARIADIC_VECTOR)
#   include <LocalBoost/boost/fusion/container/vector/detail/cpp03/vector20.hpp>
#else

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
#include <LocalBoost/boost/fusion/container/vector/vector_fwd.hpp>
#include <LocalBoost/boost/fusion/container/vector/vector.hpp>

#endif
#endif

