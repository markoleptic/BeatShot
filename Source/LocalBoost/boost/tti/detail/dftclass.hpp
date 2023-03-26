
//  (C) Copyright Edward Diener 2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_FTCLASS_HPP)
#define BOOST_TTI_DETAIL_FTCLASS_HPP

#include <LocalBoost/boost/function_types/parameter_types.hpp>
#include <LocalBoost/boost/mpl/at.hpp>
#include <LocalBoost/boost/mpl/identity.hpp>
#include <LocalBoost/boost/mpl/int.hpp>
#include <LocalBoost/boost/mpl/quote.hpp>

namespace boost
  {
  namespace tti
    {
    namespace detail
      {
      template<class BOOST_TTI_DETAIL_TP_F>
      struct class_type :
          boost::mpl::at
            <
            typename
            boost::function_types::parameter_types
              <
              BOOST_TTI_DETAIL_TP_F,
              boost::mpl::quote1
                <
                boost::mpl::identity
                >
              >::type,
              boost::mpl::int_<0>
            >
        {
        };
      }
    }
  }
  
#endif // BOOST_TTI_DETAIL_FTCLASS_HPP