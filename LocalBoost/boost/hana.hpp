/*!
@file
Includes all the library components except the adapters for external
libraries.

Copyright Louis Dionne 2013-2022
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_HPP
#define BOOST_HANA_HPP

//! @defgroup group-concepts Concepts
//! Concepts provided by the library.

//! @defgroup group-datatypes Data types
//! General purpose data types provided by the library.

//! @defgroup group-functional Functional
//! General purpose function objects.

//! @defgroup group-core Core
//! Core utilities of the library.

//! @defgroup group-experimental Experimental features
//! Experimental features that may or may not make it into the library.
//! These features should not expected to be stable.

//! @defgroup group-ext External adapters
//! Adapters for external libraries.

//! @defgroup group-config Configuration options
//! Configurable options to tweak the global behavior of the library.

//! @defgroup group-assertions Assertions
//! Macros to perform different kinds of assertions.

//! @defgroup group-details Details
//! Implementation details. Do not rely on anything here, even if it is
//! documented.

// Include config.hpp first, so that warning messages about compiler support
// appear as soon as possible.
#include <LocalBoost/boost/hana/config.hpp>


#ifdef BOOST_HANA_DOXYGEN_INVOKED
namespace boost {
    //! Namespace containing everything in the library.
    namespace hana {
        //! Namespace containing C++14 user-defined literals provided by Hana.
        namespace literals {}
    }
}
#endif

#include <LocalBoost/boost/hana/accessors.hpp>
#include <LocalBoost/boost/hana/adapt_adt.hpp>
#include <LocalBoost/boost/hana/adapt_struct.hpp>
#include <LocalBoost/boost/hana/adjust.hpp>
#include <LocalBoost/boost/hana/adjust_if.hpp>
#include <LocalBoost/boost/hana/all.hpp>
#include <LocalBoost/boost/hana/all_of.hpp>
#include <LocalBoost/boost/hana/and.hpp>
#include <LocalBoost/boost/hana/any.hpp>
#include <LocalBoost/boost/hana/any_of.hpp>
#include <LocalBoost/boost/hana/ap.hpp>
#include <LocalBoost/boost/hana/append.hpp>
#include <LocalBoost/boost/hana/assert.hpp>
#include <LocalBoost/boost/hana/at.hpp>
#include <LocalBoost/boost/hana/at_key.hpp>
#include <LocalBoost/boost/hana/back.hpp>
#include <LocalBoost/boost/hana/basic_tuple.hpp>
#include <LocalBoost/boost/hana/bool.hpp>
#include <LocalBoost/boost/hana/cartesian_product.hpp>
#include <LocalBoost/boost/hana/chain.hpp>
#include <LocalBoost/boost/hana/comparing.hpp>
#include <LocalBoost/boost/hana/concat.hpp>
#include <LocalBoost/boost/hana/concept.hpp>
#include <LocalBoost/boost/hana/contains.hpp>
#include <LocalBoost/boost/hana/core.hpp>
#include <LocalBoost/boost/hana/count.hpp>
#include <LocalBoost/boost/hana/count_if.hpp>
#include <LocalBoost/boost/hana/cycle.hpp>
#include <LocalBoost/boost/hana/define_struct.hpp>
#include <LocalBoost/boost/hana/difference.hpp>
#include <LocalBoost/boost/hana/div.hpp>
#include <LocalBoost/boost/hana/drop_back.hpp>
#include <LocalBoost/boost/hana/drop_front.hpp>
#include <LocalBoost/boost/hana/drop_front_exactly.hpp>
#include <LocalBoost/boost/hana/drop_while.hpp>
#include <LocalBoost/boost/hana/duplicate.hpp>
#include <LocalBoost/boost/hana/empty.hpp>
#include <LocalBoost/boost/hana/equal.hpp>
#include <LocalBoost/boost/hana/erase_key.hpp>
#include <LocalBoost/boost/hana/eval.hpp>
#include <LocalBoost/boost/hana/eval_if.hpp>
#include <LocalBoost/boost/hana/extend.hpp>
#include <LocalBoost/boost/hana/extract.hpp>
#include <LocalBoost/boost/hana/fill.hpp>
#include <LocalBoost/boost/hana/filter.hpp>
#include <LocalBoost/boost/hana/find.hpp>
#include <LocalBoost/boost/hana/find_if.hpp>
#include <LocalBoost/boost/hana/first.hpp>
#include <LocalBoost/boost/hana/flatten.hpp>
#include <LocalBoost/boost/hana/fold.hpp>
#include <LocalBoost/boost/hana/fold_left.hpp>
#include <LocalBoost/boost/hana/fold_right.hpp>
#include <LocalBoost/boost/hana/for_each.hpp>
#include <LocalBoost/boost/hana/front.hpp>
#include <LocalBoost/boost/hana/functional.hpp>
#include <LocalBoost/boost/hana/fuse.hpp>
#include <LocalBoost/boost/hana/greater.hpp>
#include <LocalBoost/boost/hana/greater_equal.hpp>
#include <LocalBoost/boost/hana/group.hpp>
#include <LocalBoost/boost/hana/index_if.hpp>
#include <LocalBoost/boost/hana/if.hpp>
#include <LocalBoost/boost/hana/insert.hpp>
#include <LocalBoost/boost/hana/insert_range.hpp>
#include <LocalBoost/boost/hana/integral_constant.hpp>
#include <LocalBoost/boost/hana/intersection.hpp>
#include <LocalBoost/boost/hana/intersperse.hpp>
#include <LocalBoost/boost/hana/is_disjoint.hpp>
#include <LocalBoost/boost/hana/is_empty.hpp>
#include <LocalBoost/boost/hana/is_subset.hpp>
#include <LocalBoost/boost/hana/keys.hpp>
#include <LocalBoost/boost/hana/lazy.hpp>
#include <LocalBoost/boost/hana/length.hpp>
#include <LocalBoost/boost/hana/less.hpp>
#include <LocalBoost/boost/hana/less_equal.hpp>
#include <LocalBoost/boost/hana/lexicographical_compare.hpp>
#include <LocalBoost/boost/hana/lift.hpp>
#include <LocalBoost/boost/hana/map.hpp>
#include <LocalBoost/boost/hana/max.hpp>
#include <LocalBoost/boost/hana/maximum.hpp>
#include <LocalBoost/boost/hana/members.hpp>
#include <LocalBoost/boost/hana/min.hpp>
#include <LocalBoost/boost/hana/minimum.hpp>
#include <LocalBoost/boost/hana/minus.hpp>
#include <LocalBoost/boost/hana/mod.hpp>
#include <LocalBoost/boost/hana/monadic_compose.hpp>
#include <LocalBoost/boost/hana/monadic_fold_left.hpp>
#include <LocalBoost/boost/hana/monadic_fold_right.hpp>
#include <LocalBoost/boost/hana/mult.hpp>
#include <LocalBoost/boost/hana/negate.hpp>
#include <LocalBoost/boost/hana/none.hpp>
#include <LocalBoost/boost/hana/none_of.hpp>
#include <LocalBoost/boost/hana/not.hpp>
#include <LocalBoost/boost/hana/not_equal.hpp>
#include <LocalBoost/boost/hana/one.hpp>
#include <LocalBoost/boost/hana/optional.hpp>
#include <LocalBoost/boost/hana/or.hpp>
#include <LocalBoost/boost/hana/ordering.hpp>
#include <LocalBoost/boost/hana/pair.hpp>
#include <LocalBoost/boost/hana/partition.hpp>
#include <LocalBoost/boost/hana/permutations.hpp>
#include <LocalBoost/boost/hana/plus.hpp>
#include <LocalBoost/boost/hana/power.hpp>
#include <LocalBoost/boost/hana/prefix.hpp>
#include <LocalBoost/boost/hana/prepend.hpp>
#include <LocalBoost/boost/hana/product.hpp>
#include <LocalBoost/boost/hana/range.hpp>
#include <LocalBoost/boost/hana/remove.hpp>
#include <LocalBoost/boost/hana/remove_at.hpp>
#include <LocalBoost/boost/hana/remove_if.hpp>
#include <LocalBoost/boost/hana/remove_range.hpp>
#include <LocalBoost/boost/hana/repeat.hpp>
#include <LocalBoost/boost/hana/replace.hpp>
#include <LocalBoost/boost/hana/replace_if.hpp>
#include <LocalBoost/boost/hana/replicate.hpp>
#include <LocalBoost/boost/hana/reverse.hpp>
#include <LocalBoost/boost/hana/reverse_fold.hpp>
#include <LocalBoost/boost/hana/scan_left.hpp>
#include <LocalBoost/boost/hana/scan_right.hpp>
#include <LocalBoost/boost/hana/second.hpp>
#include <LocalBoost/boost/hana/set.hpp>
#include <LocalBoost/boost/hana/size.hpp>
#include <LocalBoost/boost/hana/slice.hpp>
#include <LocalBoost/boost/hana/sort.hpp>
#include <LocalBoost/boost/hana/span.hpp>
#include <LocalBoost/boost/hana/string.hpp>
#include <LocalBoost/boost/hana/suffix.hpp>
#include <LocalBoost/boost/hana/sum.hpp>
#include <LocalBoost/boost/hana/symmetric_difference.hpp>
#include <LocalBoost/boost/hana/take_back.hpp>
#include <LocalBoost/boost/hana/take_front.hpp>
#include <LocalBoost/boost/hana/take_while.hpp>
#include <LocalBoost/boost/hana/tap.hpp>
#include <LocalBoost/boost/hana/then.hpp>
#include <LocalBoost/boost/hana/traits.hpp>
#include <LocalBoost/boost/hana/transform.hpp>
#include <LocalBoost/boost/hana/tuple.hpp>
#include <LocalBoost/boost/hana/type.hpp>
#include <LocalBoost/boost/hana/unfold_left.hpp>
#include <LocalBoost/boost/hana/unfold_right.hpp>
#include <LocalBoost/boost/hana/union.hpp>
#include <LocalBoost/boost/hana/unique.hpp>
#include <LocalBoost/boost/hana/unpack.hpp>
#include <LocalBoost/boost/hana/value.hpp>
#include <LocalBoost/boost/hana/version.hpp>
#include <LocalBoost/boost/hana/while.hpp>
#include <LocalBoost/boost/hana/zero.hpp>
#include <LocalBoost/boost/hana/zip.hpp>
#include <LocalBoost/boost/hana/zip_shortest.hpp>
#include <LocalBoost/boost/hana/zip_shortest_with.hpp>
#include <LocalBoost/boost/hana/zip_with.hpp>

#endif // !BOOST_HANA_HPP
