
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <LocalBoost/boost/config.hpp>

#if defined(BOOST_WINDOWS)
# include <LocalBoost/boost/coroutine/windows/protected_stack_allocator.hpp>
#else
# include <LocalBoost/boost/coroutine/posix/protected_stack_allocator.hpp>
#endif
