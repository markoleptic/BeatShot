/*!
@file
Defines `boost::hana::Group`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONCEPT_GROUP_HPP
#define BOOST_HANA_CONCEPT_GROUP_HPP

#include <LocalBoost/boost/hana/fwd/concept/group.hpp>

#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/default.hpp>
#include <LocalBoost/boost/hana/core/tag_of.hpp>
#include <LocalBoost/boost/hana/detail/integral_constant.hpp>
#include <LocalBoost/boost/hana/minus.hpp>
#include <LocalBoost/boost/hana/negate.hpp>


namespace boost { namespace hana {
    template <typename G>
    struct Group
        : hana::integral_constant<bool,
            !is_default<negate_impl<typename tag_of<G>::type>>::value ||
            !is_default<minus_impl<typename tag_of<G>::type,
                                   typename tag_of<G>::type>>::value
        >
    { };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONCEPT_GROUP_HPP
