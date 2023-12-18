/*!
@file
Defines `boost::hana::Logical`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONCEPT_LOGICAL_HPP
#define BOOST_HANA_CONCEPT_LOGICAL_HPP

#include <LocalBoost/boost/hana/fwd/concept/logical.hpp>

#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/default.hpp>
#include <LocalBoost/boost/hana/core/tag_of.hpp>
#include <LocalBoost/boost/hana/detail/integral_constant.hpp>
#include <LocalBoost/boost/hana/eval_if.hpp>
#include <LocalBoost/boost/hana/not.hpp>
#include <LocalBoost/boost/hana/while.hpp>


namespace boost { namespace hana {
    template <typename L>
    struct Logical
        : hana::integral_constant<bool,
            !is_default<eval_if_impl<typename tag_of<L>::type>>::value &&
            !is_default<not_impl<typename tag_of<L>::type>>::value &&
            !is_default<while_impl<typename tag_of<L>::type>>::value
        >
    { };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONCEPT_LOGICAL_HPP