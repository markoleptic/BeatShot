/*!
@file
Defines `boost::hana::replace`.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_REPLACE_HPP
#define BOOST_HANA_REPLACE_HPP

#include <LocalBoost/boost/hana/fwd/replace.hpp>

#include <LocalBoost/boost/hana/concept/functor.hpp>
#include <LocalBoost/boost/hana/config.hpp>
#include <LocalBoost/boost/hana/core/dispatch.hpp>
#include <LocalBoost/boost/hana/equal.hpp>
#include <LocalBoost/boost/hana/replace_if.hpp>


namespace boost { namespace hana {
    //! @cond
    template <typename Xs, typename OldVal, typename NewVal>
    constexpr auto replace_t::operator()(Xs&& xs, OldVal&& oldval, NewVal&& newval) const {
        using S = typename hana::tag_of<Xs>::type;
        using Replace = BOOST_HANA_DISPATCH_IF(replace_impl<S>,
            hana::Functor<S>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(hana::Functor<S>::value,
        "hana::replace(xs, oldval, newval) requires 'xs' to be a Functor");
    #endif

        return Replace::apply(static_cast<Xs&&>(xs),
                              static_cast<OldVal&&>(oldval),
                              static_cast<NewVal&&>(newval));
    }
    //! @endcond

    template <typename Fun, bool condition>
    struct replace_impl<Fun, when<condition>> : default_ {
        template <typename Xs, typename OldVal, typename NewVal>
        static constexpr decltype(auto)
        apply(Xs&& xs, OldVal&& oldval, NewVal&& newval) {
            return hana::replace_if(
                static_cast<Xs&&>(xs),
                hana::equal.to(static_cast<OldVal&&>(oldval)),
                static_cast<NewVal&&>(newval)
            );
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_REPLACE_HPP
