/*!
@file
Defines `boost::hana::Functor`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONCEPT_FUNCTOR_HPP
#define BOOST_HANA_CONCEPT_FUNCTOR_HPP

#include <LocalBoost/boost/hana/fwd/concept/functor.hpp>

#include <LocalBoost/boost/hana/adjust_if.hpp>
#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/default.hpp>
#include <LocalBoost/boost/hana/core/tag_of.hpp>
#include <LocalBoost/boost/hana/detail/integral_constant.hpp>
#include <LocalBoost/boost/hana/transform.hpp>


namespace boost { namespace hana {
    template <typename F>
    struct Functor
        : hana::integral_constant<bool,
            !is_default<transform_impl<typename tag_of<F>::type>>::value ||
            !is_default<adjust_if_impl<typename tag_of<F>::type>>::value
        >
    { };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONCEPT_FUNCTOR_HPP
