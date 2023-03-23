/*!
@file
Defines `boost::hana::all_of`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_ALL_OF_HPP
#define BOOST_HANA_ALL_OF_HPP

#include <LocalBoost/boost/hana/fwd/all_of.hpp>

#include <LocalBoost/boost/hana/any_of.hpp>
#include <LocalBoost/boost/hana/concept/searchable.hpp>
#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/dispatch.hpp>
#include <LocalBoost/boost/hana/functional/compose.hpp>
#include <LocalBoost/boost/hana/not.hpp>


namespace boost { namespace hana {
    //! @cond
    template <typename Xs, typename Pred>
    constexpr auto all_of_t::operator()(Xs&& xs, Pred&& pred) const {
        using S = typename hana::tag_of<Xs>::type;
        using AllOf = BOOST_HANA_DISPATCH_IF(all_of_impl<S>,
            hana::Searchable<S>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(hana::Searchable<S>::value,
        "hana::all_of(xs, pred) requires 'xs' to be a Searchable");
    #endif

        return AllOf::apply(static_cast<Xs&&>(xs), static_cast<Pred&&>(pred));
    }
    //! @endcond

    template <typename S, bool condition>
    struct all_of_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Pred>
        static constexpr auto apply(Xs&& xs, Pred&& pred) {
            return hana::not_(hana::any_of(static_cast<Xs&&>(xs),
                    hana::compose(hana::not_, static_cast<Pred&&>(pred))));
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_ALL_OF_HPP
