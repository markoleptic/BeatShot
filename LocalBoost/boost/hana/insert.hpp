/*!
@file
Defines `boost::hana::insert`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_INSERT_HPP
#define BOOST_HANA_INSERT_HPP

#include <LocalBoost/boost/hana/fwd/insert.hpp>

#include <LocalBoost/boost/hana/append.hpp>
#include <LocalBoost/boost/hana/concat.hpp>
#include <LocalBoost/boost/hana/concept/sequence.hpp>
#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/dispatch.hpp>
#include <LocalBoost/boost/hana/drop_front.hpp>
#include <LocalBoost/boost/hana/take_front.hpp>


namespace boost { namespace hana {
    //! @cond
    template <typename Set, typename ...Args>
    constexpr decltype(auto) insert_t::operator()(Set&& set, Args&& ...args) const {
        return insert_impl<typename hana::tag_of<Set>::type>::apply(
            static_cast<Set&&>(set),
            static_cast<Args&&>(args)...
        );
    }
    //! @endcond

    template <typename T, bool condition>
    struct insert_impl<T, when<condition>> : default_ {
        template <typename ...Args>
        static constexpr auto apply(Args&& ...) = delete;
    };

    template <typename S>
    struct insert_impl<S, when<Sequence<S>::value>> {
        template <typename Xs, typename N, typename Element>
        static constexpr auto apply(Xs&& xs, N const& n, Element&& e) {
            return hana::concat(hana::append(hana::take_front(xs, n),
                                             static_cast<Element&&>(e)),
                                hana::drop_front(xs, n));
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_INSERT_HPP
