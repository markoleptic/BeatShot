// Copyright David Abrahams 2005.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP
#define BOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP

#include <LocalBoost/boost/parameter/aux_/use_default_tag.hpp>
#include <LocalBoost/boost/parameter/config.hpp>
#include <LocalBoost/boost/utility/result_of.hpp>

#if defined(BOOST_PARAMETER_CAN_USE_MP11)
#include <LocalBoost/boost/mp11/utility.hpp>
#include <type_traits>
#else
#include <LocalBoost/boost/mpl/if.hpp>
#include <LocalBoost/boost/type_traits/is_void.hpp>
#endif

namespace boost { namespace parameter { namespace aux {

    // A metafunction returning the result of invoking
    // a nullary function object of the given type.
    template <typename F>
    class result_of0
    {
#if defined(BOOST_NO_RESULT_OF)
        typedef typename F::result_type result_of_F;
#else
        typedef typename ::boost::result_of<F()>::type result_of_F;
#endif

     public:
#if defined(BOOST_PARAMETER_CAN_USE_MP11)
        using type = ::boost::mp11::mp_if<
            ::std::is_void<result_of_F>
#else
        typedef typename ::boost::mpl::if_<
            ::boost::is_void<result_of_F>
#endif
          , ::boost::parameter::aux::use_default_tag
          , result_of_F
#if defined(BOOST_PARAMETER_CAN_USE_MP11)
        >;
#else
        >::type type;
#endif
    };
}}} // namespace boost::parameter::aux

#endif  // include guard
