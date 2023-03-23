// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_EXT_PUSH_BACK_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_EXT_PUSH_BACK_HPP_INCLUDED

#include <LocalBoost/boost/range/config.hpp>
#include <LocalBoost/boost/range/concepts.hpp>
#include <LocalBoost/boost/range/difference_type.hpp>
#include <LocalBoost/boost/range/begin.hpp>
#include <LocalBoost/boost/range/end.hpp>
#include <LocalBoost/boost/range/detail/implementation_help.hpp>
#include <LocalBoost/boost/assert.hpp>

namespace boost
{
    namespace range
    {

template< class Container, class Range >
inline Container& push_back( Container& on, const Range& from )
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<Container> ));
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const Range> ));
    BOOST_ASSERT_MSG(!range_detail::is_same_object(on, from),
        "cannot copy from a container to itself");
    on.insert( on.end(), boost::begin(from), boost::end(from) );
    return on;
}

    } // namespace range
    using range::push_back;
} // namespace boost

#endif // include guard
