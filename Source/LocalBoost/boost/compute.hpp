//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_HPP
#define BOOST_COMPUTE_HPP

#include <LocalBoost/boost/compute/algorithm.hpp>
#include <LocalBoost/boost/compute/allocator.hpp>
#include <LocalBoost/boost/compute/async.hpp>
#include <LocalBoost/boost/compute/buffer.hpp>
#include <LocalBoost/boost/compute/cl.hpp>
#include <LocalBoost/boost/compute/command_queue.hpp>
#include <LocalBoost/boost/compute/config.hpp>
#include <LocalBoost/boost/compute/container.hpp>
#include <LocalBoost/boost/compute/context.hpp>
#include <LocalBoost/boost/compute/device.hpp>
#include <LocalBoost/boost/compute/functional.hpp>
#include <LocalBoost/boost/compute/image.hpp>
#include <LocalBoost/boost/compute/iterator.hpp>
#include <LocalBoost/boost/compute/kernel.hpp>
#include <LocalBoost/boost/compute/lambda.hpp>
#include <LocalBoost/boost/compute/pipe.hpp>
#include <LocalBoost/boost/compute/platform.hpp>
#include <LocalBoost/boost/compute/program.hpp>
#include <LocalBoost/boost/compute/random.hpp>
#include <LocalBoost/boost/compute/svm.hpp>
#include <LocalBoost/boost/compute/system.hpp>
#include <LocalBoost/boost/compute/types.hpp>
#include <LocalBoost/boost/compute/user_event.hpp>
#include <LocalBoost/boost/compute/utility.hpp>
#include <LocalBoost/boost/compute/version.hpp>

#ifdef BOOST_COMPUTE_HAVE_HDR_CL_EXT
#include <LocalBoost/boost/compute/cl_ext.hpp>
#endif

#endif // BOOST_COMPUTE_HPP
