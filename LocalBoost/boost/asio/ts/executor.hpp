//
// ts/executor.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_TS_EXECUTOR_HPP
#define BOOST_ASIO_TS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <LocalBoost/boost/asio/async_result.hpp>
#include <LocalBoost/boost/asio/associated_allocator.hpp>
#include <LocalBoost/boost/asio/execution_context.hpp>
#include <LocalBoost/boost/asio/is_executor.hpp>
#include <LocalBoost/boost/asio/associated_executor.hpp>
#include <LocalBoost/boost/asio/bind_executor.hpp>
#include <LocalBoost/boost/asio/executor_work_guard.hpp>
#include <LocalBoost/boost/asio/system_executor.hpp>
#include <LocalBoost/boost/asio/executor.hpp>
#include <LocalBoost/boost/asio/any_io_executor.hpp>
#include <LocalBoost/boost/asio/dispatch.hpp>
#include <LocalBoost/boost/asio/post.hpp>
#include <LocalBoost/boost/asio/defer.hpp>
#include <LocalBoost/boost/asio/strand.hpp>
#include <LocalBoost/boost/asio/packaged_task.hpp>
#include <LocalBoost/boost/asio/use_future.hpp>

#endif // BOOST_ASIO_TS_EXECUTOR_HPP
