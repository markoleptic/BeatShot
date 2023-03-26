//
// detail/timer_scheduler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP
#define BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <LocalBoost/boost/asio/detail/config.hpp>
#include <LocalBoost/boost/asio/detail/timer_scheduler_fwd.hpp>

#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
# include <LocalBoost/boost/asio/detail/winrt_timer_scheduler.hpp>
#elif defined(BOOST_ASIO_HAS_IOCP)
# include <LocalBoost/boost/asio/detail/win_iocp_io_context.hpp>
#elif defined(BOOST_ASIO_HAS_IO_URING_AS_DEFAULT)
# include <LocalBoost/boost/asio/detail/io_uring_service.hpp>
#elif defined(BOOST_ASIO_HAS_EPOLL)
# include <LocalBoost/boost/asio/detail/epoll_reactor.hpp>
#elif defined(BOOST_ASIO_HAS_KQUEUE)
# include <LocalBoost/boost/asio/detail/kqueue_reactor.hpp>
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
# include <LocalBoost/boost/asio/detail/dev_poll_reactor.hpp>
#else
# include <LocalBoost/boost/asio/detail/select_reactor.hpp>
#endif

#endif // BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP