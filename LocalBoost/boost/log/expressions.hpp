/*
 *          Copyright Andrey Semashev 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   expressions.hpp
 * \author Andrey Semashev
 * \date   10.11.2012
 *
 * This header includes other Boost.Log headers with all template expression tools.
 */

#ifndef BOOST_LOG_EXPRESSIONS_HPP_INCLUDED_
#define BOOST_LOG_EXPRESSIONS_HPP_INCLUDED_

#include <LocalBoost/boost/log/detail/config.hpp>

#include <LocalBoost/boost/log/expressions/attr.hpp>
#include <LocalBoost/boost/log/expressions/keyword.hpp>
#include <LocalBoost/boost/log/expressions/message.hpp>
#include <LocalBoost/boost/log/expressions/record.hpp>

#include <LocalBoost/boost/log/expressions/predicates.hpp>
#include <LocalBoost/boost/log/expressions/formatters.hpp>

#include <LocalBoost/boost/log/expressions/filter.hpp>
#include <LocalBoost/boost/log/expressions/formatter.hpp>

// Boost.Phoenix operators are likely to be used with Boost.Log expression nodes anyway
#include <LocalBoost/boost/phoenix/operator.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#endif // BOOST_LOG_EXPRESSIONS_HPP_INCLUDED_
