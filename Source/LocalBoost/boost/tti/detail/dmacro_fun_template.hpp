
//  (C) Copyright Edward Diener 2019
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_MACRO_FUN_TEMPLATE_HPP)
#define BOOST_TTI_DETAIL_MACRO_FUN_TEMPLATE_HPP

#include <LocalBoost/boost/preprocessor/comparison/equal.hpp>
#include <LocalBoost/boost/preprocessor/control/iif.hpp>
#include <LocalBoost/boost/preprocessor/logical/and.hpp>
#include <LocalBoost/boost/preprocessor/variadic/elem.hpp>
#include <LocalBoost/boost/preprocessor/variadic/size.hpp>
#include <LocalBoost/boost/preprocessor/variadic/to_array.hpp>
#include <LocalBoost/boost/preprocessor/detail/is_binary.hpp>
#include <LocalBoost/boost/tti/detail/dmacro_fve.hpp>

#define BOOST_TTI_DETAIL_FUN_TEMPLATE_VARIADIC_TO_ARRAY(...) \
  BOOST_PP_IIF \
    ( \
    BOOST_PP_AND \
      ( \
      BOOST_PP_EQUAL \
        ( \
        BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), \
        1 \
        ), \
      BOOST_PP_IS_BINARY \
        ( \
        BOOST_PP_VARIADIC_ELEM(0,__VA_ARGS__) \
        ) \
      ), \
    BOOST_TTI_DETAIL_FIRST_VARIADIC_ELEM, \
    BOOST_PP_VARIADIC_TO_ARRAY \
    ) \
  (__VA_ARGS__) \
/**/

#endif // BOOST_TTI_DETAIL_MACRO_FUN_TEMPLATE_HPP
