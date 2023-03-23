#ifndef BOOST_METAPARSE_V1_IMPL_FWD_ITERATE_IMPL_HPP
#define BOOST_METAPARSE_V1_IMPL_FWD_ITERATE_IMPL_HPP

// Copyright Abel Sinkovics (abel@sinkovics.hu)  2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <LocalBoost/boost/metaparse/v1/is_error.hpp>
#include <LocalBoost/boost/metaparse/v1/get_result.hpp>
#include <LocalBoost/boost/metaparse/v1/get_remaining.hpp>
#include <LocalBoost/boost/metaparse/v1/get_position.hpp>

#include <LocalBoost/boost/mpl/deque.hpp>
#include <LocalBoost/boost/mpl/eval_if.hpp>
#include <LocalBoost/boost/mpl/push_back.hpp>

namespace boost
{
  namespace metaparse
  {
    namespace v1
    {
      namespace impl
      {
        template <int N, class P, class Accum>
        struct iterate_impl;
      }
    }
  }
}

#endif

