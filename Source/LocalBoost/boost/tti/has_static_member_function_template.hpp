
//  (C) Copyright Edward Diener 2019
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_STATIC_MEM_FUN_TEMPLATE_HPP)
#define BOOST_TTI_STATIC_MEM_FUN_TEMPLATE_HPP

#include <LocalBoost/boost/config.hpp>
#include <LocalBoost/boost/function_types/property_tags.hpp>
#include <LocalBoost/boost/mpl/vector.hpp>
#include <LocalBoost/boost/preprocessor/cat.hpp>
#include <LocalBoost/boost/tti/gen/namespace_gen.hpp>
#include <LocalBoost/boost/tti/gen/has_static_member_function_template_gen.hpp>
#include <LocalBoost/boost/tti/detail/dstatic_mem_fun_template.hpp>

#if BOOST_PP_VARIADICS

#include <LocalBoost/boost/tti/detail/dmacro_fun_template.hpp>

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/// A macro which expands to a metafunction which tests whether an inner static member function template with a particular name exists.
/**

    BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE is a macro which expands to a metafunction.
    The metafunction tests whether an inner static member function template with a particular name exists.
    The macro takes the form of BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(trait,name,...) where
    
    trait = the name of the metafunction <br/>
    name  = inner static member function template name <br/>
    ...   = variadic parameters.
    
    The variadic parameter(s) are either:
    
    A sequence of valid instantiations for the static member function template parameters
    ie. 'int,long,double' etc.
        
    or
    
    A single variadic parameter which is a Boost PP array whose elements are
    a sequence of valid instantiations for the static member function template parameters
    ie. '(3,(int,long,double))' etc. This form is allowed in order to be compatible
    with using the non-variadic form of this macro.
    
    BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE generates a metafunction called "trait" where 'trait' is the first macro parameter.
    
  @code
  
            template<class BOOST_TTI_TP_T,class BOOST_TTI_R,class BOOST_TTI_FS,class BOOST_TTI_TAG>
            struct trait
              {
              static const value = unspecified;
              typedef mpl::bool_<true-or-false> type;
              };
      
            The metafunction types and return:
      
              BOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                                 The enclosing type can be a class, struct, or union.
              
              BOOST_TTI_TP_R   = the return type of the static member function template
                                 in a single instantiation of the static member function template
                                          OR
                                 a pointer to function as a single type
                                 which encapsulates a single instantiation of
                                 the static member function template.
              
              BOOST_TTI_TP_FS  = (optional) the parameters of the static member function template as a boost::mpl forward sequence
                                 if the second parameter is the enclosing type and the static member function template parameters
                                 are not empty. These parameters are a single instantiation of the static member function template.
              
              BOOST_TTI_TP_TAG = (optional) a boost::function_types tag to apply to the static member function template
                                 if the second parameter is the enclosing type and a tag is needed.
              
              returns = 'value' is true if the 'name' exists, 
                        with the appropriate static member function template type,
                        otherwise 'value' is false.
                          
  @endcode
  
*/
#define BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(trait,name,...) \
  BOOST_TTI_DETAIL_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE \
    ( \
    trait, \
    name, \
    BOOST_TTI_DETAIL_FUN_TEMPLATE_VARIADIC_TO_ARRAY(__VA_ARGS__) \
    ) \
  template<class BOOST_TTI_TP_T,class BOOST_TTI_TP_R,class BOOST_TTI_TP_FS = boost::mpl::vector<>,class BOOST_TTI_TP_TAG = boost::function_types::null_tag> \
  struct trait \
    { \
    typedef typename \
    BOOST_PP_CAT(trait,_detail_hsmft)<BOOST_TTI_TP_T,BOOST_TTI_TP_R,BOOST_TTI_TP_FS,BOOST_TTI_TP_TAG>::type type; \
    BOOST_STATIC_CONSTANT(bool,value=type::value); \
    }; \
/**/


/// A macro which expands to a metafunction which tests whether an inner static member function template with a particular name exists.
/**

    BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE is a macro which expands to a metafunction.
    The metafunction tests whether an inner static member function template with a particular name exists.
    The macro takes the form of BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(name,...) where
    
    name  = inner static member function template name <br/>
    ...   = variadic parameters.
    
    The variadic parameter(s) are either:
    
    A sequence of valid instantiations for the static member function template parameters
    ie. 'int,long,double' etc.
        
    or
    
    A single variadic parameter which is a Boost PP array whose elements are
    a sequence of valid instantiations for the static member function template parameters
    ie. '(3,(int,long,double))' etc. This form is allowed in order to be compatible
    with using the non-variadic form of this macro.
    
    BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE generates a metafunction called "has_static_member_function_template_'name'" where 'name' is the first macro parameter.
    
  @code
  
            template<class BOOST_TTI_TP_T,class BOOST_TTI_R,class BOOST_TTI_FS,class BOOST_TTI_TAG>
            struct has_static_member_function_template_'name'
              {
              static const value = unspecified;
              typedef mpl::bool_<true-or-false> type;
              };
              
            The metafunction types and return:
      
              BOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                                 The enclosing type can be a class, struct, or union.
              
              BOOST_TTI_TP_R   = the return type of the static member function template
                                 in a single instantiation of the static member function template
                                          OR
                                 a pointer to function as a single type
                                 which encapsulates a single instantiation of
                                 the static member function template.
              
              BOOST_TTI_TP_FS  = (optional) the parameters of the static member function template as a boost::mpl forward sequence
                                 if the second parameter is the enclosing type and the static member function template parameters
                                 are not empty. These parameters are a single instantiation of the static member function template.
              
              BOOST_TTI_TP_TAG = (optional) a boost::function_types tag to apply to the static member function template
                                 if the second parameter is the enclosing type and a tag is needed.
              
              returns = 'value' is true if the 'name' exists, 
                        with the appropriate static member function template type,
                        otherwise 'value' is false.
                          
  @endcode
  
*/
#define BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(name,...) \
  BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE \
  ( \
  BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE_GEN(name), \
  name, \
  __VA_ARGS__ \
  ) \
/**/

#else // !BOOST_PP_VARIADICS

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/// A macro which expands to a metafunction which tests whether an inner static member function template with a particular name exists.
/**

    BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE is a macro which expands to a metafunction.
    The metafunction tests whether an inner static member function template with a particular name exists.
    The macro takes the form of BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(trait,name,pparray) where
    
    trait   = the name of the metafunction <br/>
    name    = inner static member function template name <br/>
    pparray = a Boost PP array whose elements are a sequence of valid instantiations for
            the static member function template parameters ie. '(3,(int,long,double))' etc.
    
    BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE generates a metafunction called "trait" where 'trait' is the first macro parameter.
    
  @code
  
            template<class BOOST_TTI_TP_T,class BOOST_TTI_R,class BOOST_TTI_FS,class BOOST_TTI_TAG>
            struct trait
              {
              static const value = unspecified;
              typedef mpl::bool_<true-or-false> type;
              };
      
            The metafunction types and return:
      
              BOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                                 The enclosing type can be a class, struct, or union.
              
              BOOST_TTI_TP_R   = the return type of the static member function template
                                 in a single instantiation of the static member function template
                                          OR
                                 a pointer to function as a single type
                                 which encapsulates a single instantiation of
                                 the static member function template.
              
              BOOST_TTI_TP_FS  = (optional) the parameters of the static member function template as a boost::mpl forward sequence
                                 if the second parameter is the enclosing type and the static member function template parameters
                                 are not empty. These parameters are a single instantiation of the static member function template.
              
              BOOST_TTI_TP_TAG = (optional) a boost::function_types tag to apply to the static member function template
                                 if the second parameter is the enclosing type and a tag is needed.
              
              returns = 'value' is true if the 'name' exists, 
                        with the appropriate static member function template type,
                        otherwise 'value' is false.
                          
  @endcode
  
*/
#define BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(trait,name,pparray) \
  BOOST_TTI_DETAIL_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(trait,name,pparray) \
  template<class BOOST_TTI_TP_T,class BOOST_TTI_TP_R,class BOOST_TTI_TP_FS = boost::mpl::vector<>,class BOOST_TTI_TP_TAG = boost::function_types::null_tag> \
  struct trait \
    { \
    typedef typename \
    BOOST_PP_CAT(trait,_detail_hsmft)<BOOST_TTI_TP_T,BOOST_TTI_TP_R,BOOST_TTI_TP_FS,BOOST_TTI_TP_TAG>::type type; \
    BOOST_STATIC_CONSTANT(bool,value=type::value); \
    }; \
/**/

/// A macro which expands to a metafunction which tests whether an inner static member function template with a particular name exists.
/**

    BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE is a macro which expands to a metafunction.
    The metafunction tests whether an inner static member function template with a particular name exists.
    The macro takes the form of BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(name,pparray) where
    
    name    = inner static member function template name <br/>
    pparray = a Boost PP array whose elements are a sequence of valid instantiations for
              the static member function template parameters ie. '(3,(int,long,double))' etc.
    
    BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE generates a metafunction called "has_static_member_function_template_'name'" where 'name' is the first macro parameter.
    
  @code
  
            template<class BOOST_TTI_TP_T,class BOOST_TTI_R,class BOOST_TTI_FS,class BOOST_TTI_TAG>
            struct has_static_member_function_template_'name'
              {
              static const value = unspecified;
              typedef mpl::bool_<true-or-false> type;
              };
              
            The metafunction types and return:
      
              BOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                                 The enclosing type can be a class, struct, or union.
              
              BOOST_TTI_TP_R   = the return type of the static member function template
                                 in a single instantiation of the static member function template
                                          OR
                                 a pointer to function as a single type
                                 which encapsulates a single instantiation of
                                 the static member function template.
              
              BOOST_TTI_TP_FS  = (optional) the parameters of the static member function template as a boost::mpl forward sequence
                                 if the second parameter is the enclosing type and the static member function template parameters
                                 are not empty. These parameters are a single instantiation of the static member function template.
              
              BOOST_TTI_TP_TAG = (optional) a boost::function_types tag to apply to the static member function template
                                 if the second parameter is the enclosing type and a tag is needed.
              
              returns = 'value' is true if the 'name' exists, 
                        with the appropriate static member function template type,
                        otherwise 'value' is false.
                          
  @endcode
  
*/
#define BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE(name,pparray) \
  BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE \
  ( \
  BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION_TEMPLATE_GEN(name), \
  name, \
  pparray \
  ) \
/**/

#endif // BOOST_PP_VARIADICS

#endif // BOOST_TTI_STATIC_MEM_FUN_TEMPLATE_HPP
