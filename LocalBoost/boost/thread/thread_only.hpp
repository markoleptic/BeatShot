#ifndef BOOST_THREAD_THREAD_ONLY_HPP
#define BOOST_THREAD_THREAD_ONLY_HPP

//  thread.hpp
//
//  (C) Copyright 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <LocalBoost/boost/thread/detail/platform.hpp>

#if defined(BOOST_THREAD_PLATFORM_WIN32)
#include <LocalBoost/boost/thread/win32/thread_data.hpp>
#elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
#include <LocalBoost/boost/thread/pthread/thread_data.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#include <LocalBoost/boost/thread/detail/thread.hpp>
#if defined BOOST_THREAD_PROVIDES_INTERRUPTIONS
#include <LocalBoost/boost/thread/detail/thread_interruption.hpp>
#endif
#include <LocalBoost/boost/thread/condition_variable.hpp>


#endif
