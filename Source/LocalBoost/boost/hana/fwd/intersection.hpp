/*!
@file
Forward declares `boost::hana::intersection`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_FWD_INTERSECTION_HPP
#define BOOST_HANA_FWD_INTERSECTION_HPP

#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/when.hpp>


namespace boost { namespace hana {
    // Note: This function is documented per datatype/concept only.
    //! @cond
    template <typename S, typename = void>
    struct intersection_impl : intersection_impl<S, when<true>> { };
    //! @endcond

    struct intersection_t {
        template <typename Xs, typename Ys>
        constexpr auto operator()(Xs&&, Ys&&) const;
    };

    BOOST_HANA_INLINE_VARIABLE constexpr intersection_t intersection{};
}} // end namespace boost::hana

#endif // !BOOST_HANA_FWD_INTERSECTION_HPP
