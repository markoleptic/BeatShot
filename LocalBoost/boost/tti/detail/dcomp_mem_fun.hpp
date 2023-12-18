
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP)
#define BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP

#include <LocalBoost/boost/mpl/bool.hpp>
#include <LocalBoost/boost/preprocessor/cat.hpp>
#include <LocalBoost/boost/tti/detail/dftclass.hpp>
#include <LocalBoost/boost/tti/detail/dmacro_sunfix.hpp>
#include <LocalBoost/boost/tti/detail/dnullptr.hpp>
#include <LocalBoost/boost/tti/gen/namespace_gen.hpp>
#include <LocalBoost/boost/type_traits/remove_const.hpp>
#include <LocalBoost/boost/type_traits/detail/yes_no_type.hpp>

#define BOOST_TTI_DETAIL_TRAIT_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_detail_hcmf) \
    { \
    template<class BOOST_TTI_DETAIL_TP_F> \
    struct cl_type : \
      boost::remove_const \
        < \
        typename BOOST_TTI_NAMESPACE::detail::class_type<BOOST_TTI_DETAIL_TP_F>::type \
        > \
      { \
      }; \
    \
    template<BOOST_TTI_DETAIL_TP_T> \
    struct helper BOOST_TTI_DETAIL_MACRO_SUNFIX ; \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::boost::type_traits::yes_type chkt(helper<&BOOST_TTI_DETAIL_TP_U::name> *); \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::boost::type_traits::no_type chkt(...); \
    \
    typedef boost::mpl::bool_<sizeof(chkt<typename cl_type<BOOST_TTI_DETAIL_TP_T>::type>(BOOST_TTI_DETAIL_NULLPTR))==sizeof(::boost::type_traits::yes_type)> type; \
    }; \
/**/

#endif // BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP
