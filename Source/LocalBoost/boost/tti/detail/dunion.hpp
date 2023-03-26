
//  (C) Copyright Edward Diener 2019
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_UNION_HPP)
#define BOOST_TTI_DETAIL_UNION_HPP

#include <LocalBoost/boost/mpl/and.hpp>
#include <LocalBoost/boost/mpl/apply.hpp>
#include <LocalBoost/boost/mpl/bool.hpp>
#include <LocalBoost/boost/mpl/eval_if.hpp>
#include <LocalBoost/boost/mpl/has_xxx.hpp>
#include <LocalBoost/boost/preprocessor/cat.hpp>
#include <LocalBoost/boost/tti/detail/ddeftype.hpp>
#include <LocalBoost/boost/tti/detail/dlambda.hpp>
#include <LocalBoost/boost/tti/detail/denclosing_type.hpp>
#include <LocalBoost/boost/tti/gen/namespace_gen.hpp>
#include <LocalBoost/boost/type_traits/is_union.hpp>

#define BOOST_TTI_DETAIL_TRAIT_INVOKE_HAS_UNION(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_MFC> \
struct BOOST_PP_CAT(trait,_detail_union_invoke) \
  { \
  BOOST_MPL_ASSERT((BOOST_TTI_NAMESPACE::detail::is_lambda_expression<BOOST_TTI_DETAIL_TP_MFC>)); \
  typedef typename boost::mpl::apply<BOOST_TTI_DETAIL_TP_MFC,typename BOOST_TTI_DETAIL_TP_T::name>::type type; \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_UNION_OP_CHOOSE(trait,name) \
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(BOOST_PP_CAT(trait,_detail_union_mpl), name, false) \
BOOST_TTI_DETAIL_TRAIT_INVOKE_HAS_UNION(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U,class BOOST_TTI_DETAIL_TP_B> \
struct BOOST_PP_CAT(trait,_detail_union_op_choose) : \
  boost::mpl::and_ \
    < \
    boost::is_union<typename BOOST_TTI_DETAIL_TP_T::name>, \
    BOOST_PP_CAT(trait,_detail_union_invoke)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U> \
    > \
  { \
  }; \
\
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_union_op_choose)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U,boost::mpl::false_::type> : \
  boost::mpl::false_ \
  { \
  }; \
\
template<class BOOST_TTI_DETAIL_TP_T> \
struct BOOST_PP_CAT(trait,_detail_union_op_choose)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_NAMESPACE::detail::deftype,boost::mpl::true_::type> : \
  boost::is_union<typename BOOST_TTI_DETAIL_TP_T::name> \
  { \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_UNION_OP(trait,name) \
BOOST_TTI_DETAIL_TRAIT_HAS_UNION_OP_CHOOSE(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_union_op) : \
  BOOST_PP_CAT(trait,_detail_union_op_choose) \
    < \
    BOOST_TTI_DETAIL_TP_T, \
    BOOST_TTI_DETAIL_TP_U, \
    typename BOOST_PP_CAT(trait,_detail_union_mpl)<BOOST_TTI_DETAIL_TP_T>::type \
    > \
  { \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_UNION(trait,name) \
BOOST_TTI_DETAIL_TRAIT_HAS_UNION_OP(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_union) : \
    boost::mpl::eval_if \
        < \
        BOOST_TTI_NAMESPACE::detail::enclosing_type<BOOST_TTI_DETAIL_TP_T>, \
        BOOST_PP_CAT(trait,_detail_union_op)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U>, \
        boost::mpl::false_ \
        > \
  { \
  }; \
/**/

#endif // BOOST_TTI_DETAIL_UNION_HPP