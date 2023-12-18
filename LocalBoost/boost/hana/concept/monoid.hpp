/*!
@file
Defines `boost::hana::Monoid`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONCEPT_MONOID_HPP
#define BOOST_HANA_CONCEPT_MONOID_HPP

#include <LocalBoost/boost/hana/fwd/concept/monoid.hpp>

#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/default.hpp>
#include <LocalBoost/boost/hana/core/tag_of.hpp>
#include <LocalBoost/boost/hana/detail/integral_constant.hpp>
#include <LocalBoost/boost/hana/plus.hpp>
#include <LocalBoost/boost/hana/zero.hpp>


namespace boost { namespace hana {
    template <typename M>
    struct Monoid
        : hana::integral_constant<bool,
            !is_default<zero_impl<typename tag_of<M>::type>>::value &&
            !is_default<plus_impl<typename tag_of<M>::type,
                                  typename tag_of<M>::type>>::value
        >
    { };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONCEPT_MONOID_HPP
