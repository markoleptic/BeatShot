// Copyright David Abrahams, Daniel Wallin 2003.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_AUX_PACK_MAKE_DEDUCED_ITEMS_HPP
#define BOOST_PARAMETER_AUX_PACK_MAKE_DEDUCED_ITEMS_HPP

#include <LocalBoost/boost/parameter/aux_/void.hpp>
#include <LocalBoost/boost/parameter/aux_/pack/deduced_item.hpp>
#include <LocalBoost/boost/parameter/deduced.hpp>
#include <LocalBoost/boost/parameter/config.hpp>

#if defined(BOOST_PARAMETER_CAN_USE_MP11)
#include <LocalBoost/boost/mp11/utility.hpp>
#include <type_traits>
#else
#include <LocalBoost/boost/mpl/eval_if.hpp>
#include <LocalBoost/boost/mpl/identity.hpp>
#include <LocalBoost/boost/type_traits/is_same.hpp>
#endif

namespace boost { namespace parameter { namespace aux {

    template <typename Spec, typename Tail>
#if defined(BOOST_PARAMETER_CAN_USE_MP11)
    using make_deduced_items = ::boost::mp11::mp_if<
        ::std::is_same<Spec,::boost::parameter::void_>
      , ::boost::mp11::mp_identity< ::boost::parameter::void_>
      , ::boost::mp11::mp_if<
            ::boost::parameter::aux::is_deduced<Spec>
          , ::boost::parameter::aux::make_deduced_item<Spec,Tail>
          , Tail
        >
    >;
#else
    struct make_deduced_items
      : ::boost::mpl::eval_if<
            ::boost::is_same<Spec,::boost::parameter::void_>
          , ::boost::mpl::identity< ::boost::parameter::void_>
          , ::boost::mpl::eval_if<
                ::boost::parameter::aux::is_deduced<Spec>
              , ::boost::parameter::aux::make_deduced_item<Spec,Tail>
              , Tail
            >
        >
    {
    };
#endif  // BOOST_PARAMETER_CAN_USE_MP11
}}} // namespace boost::parameter::aux

#endif  // include guard

