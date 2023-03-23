//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_OPTIONAL_HPP
#define BOOST_URL_OPTIONAL_HPP

#include <LocalBoost/boost/url/detail/config.hpp>
#include <LocalBoost/boost/optional.hpp>

namespace boost {
namespace urls {

/** The type of optional used by the library
*/
template<class T>
using optional = boost::optional<T>;

} // urls
} // boost

#endif
