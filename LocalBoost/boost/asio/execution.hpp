//
// execution.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_EXECUTION_HPP
#define BOOST_ASIO_EXECUTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <LocalBoost/boost/asio/execution/allocator.hpp>
#include <LocalBoost/boost/asio/execution/any_executor.hpp>
#include <LocalBoost/boost/asio/execution/bad_executor.hpp>
#include <LocalBoost/boost/asio/execution/blocking.hpp>
#include <LocalBoost/boost/asio/execution/blocking_adaptation.hpp>
#include <LocalBoost/boost/asio/execution/bulk_execute.hpp>
#include <LocalBoost/boost/asio/execution/bulk_guarantee.hpp>
#include <LocalBoost/boost/asio/execution/connect.hpp>
#include <LocalBoost/boost/asio/execution/context.hpp>
#include <LocalBoost/boost/asio/execution/context_as.hpp>
#include <LocalBoost/boost/asio/execution/execute.hpp>
#include <LocalBoost/boost/asio/execution/executor.hpp>
#include <LocalBoost/boost/asio/execution/invocable_archetype.hpp>
#include <LocalBoost/boost/asio/execution/mapping.hpp>
#include <LocalBoost/boost/asio/execution/occupancy.hpp>
#include <LocalBoost/boost/asio/execution/operation_state.hpp>
#include <LocalBoost/boost/asio/execution/outstanding_work.hpp>
#include <LocalBoost/boost/asio/execution/prefer_only.hpp>
#include <LocalBoost/boost/asio/execution/receiver.hpp>
#include <LocalBoost/boost/asio/execution/receiver_invocation_error.hpp>
#include <LocalBoost/boost/asio/execution/relationship.hpp>
#include <LocalBoost/boost/asio/execution/schedule.hpp>
#include <LocalBoost/boost/asio/execution/scheduler.hpp>
#include <LocalBoost/boost/asio/execution/sender.hpp>
#include <LocalBoost/boost/asio/execution/set_done.hpp>
#include <LocalBoost/boost/asio/execution/set_error.hpp>
#include <LocalBoost/boost/asio/execution/set_value.hpp>
#include <LocalBoost/boost/asio/execution/start.hpp>
#include <LocalBoost/boost/asio/execution/submit.hpp>

#endif // BOOST_ASIO_EXECUTION_HPP
