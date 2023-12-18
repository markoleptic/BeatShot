//  (C) Copyright John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

//  See boost/type_traits/*.hpp for full copyright notices.

#ifndef BOOST_TYPE_TRAITS_HPP
#define BOOST_TYPE_TRAITS_HPP

#include <LocalBoost/boost/type_traits/add_const.hpp>
#include <LocalBoost/boost/type_traits/add_cv.hpp>
#include <LocalBoost/boost/type_traits/add_lvalue_reference.hpp>
#include <LocalBoost/boost/type_traits/add_pointer.hpp>
#include <LocalBoost/boost/type_traits/add_reference.hpp>
#include <LocalBoost/boost/type_traits/add_rvalue_reference.hpp>
#include <LocalBoost/boost/type_traits/add_volatile.hpp>
#include <LocalBoost/boost/type_traits/aligned_storage.hpp>
#include <LocalBoost/boost/type_traits/alignment_of.hpp>
#include <LocalBoost/boost/type_traits/common_type.hpp>
#include <LocalBoost/boost/type_traits/conditional.hpp>
#include <LocalBoost/boost/type_traits/conjunction.hpp>
#include <LocalBoost/boost/type_traits/copy_cv.hpp>
#include <LocalBoost/boost/type_traits/copy_cv_ref.hpp>
#include <LocalBoost/boost/type_traits/copy_reference.hpp>
#include <LocalBoost/boost/type_traits/decay.hpp>
#include <LocalBoost/boost/type_traits/declval.hpp>
#include <LocalBoost/boost/type_traits/disjunction.hpp>
#include <LocalBoost/boost/type_traits/enable_if.hpp>
#include <LocalBoost/boost/type_traits/extent.hpp>
#include <LocalBoost/boost/type_traits/floating_point_promotion.hpp>
#include <LocalBoost/boost/type_traits/function_traits.hpp>

#include <LocalBoost/boost/type_traits/has_bit_and.hpp>
#include <LocalBoost/boost/type_traits/has_bit_and_assign.hpp>
#include <LocalBoost/boost/type_traits/has_bit_or.hpp>
#include <LocalBoost/boost/type_traits/has_bit_or_assign.hpp>
#include <LocalBoost/boost/type_traits/has_bit_xor.hpp>
#include <LocalBoost/boost/type_traits/has_bit_xor_assign.hpp>
#include <LocalBoost/boost/type_traits/has_complement.hpp>
#include <LocalBoost/boost/type_traits/has_dereference.hpp>
#include <LocalBoost/boost/type_traits/has_divides.hpp>
#include <LocalBoost/boost/type_traits/has_divides_assign.hpp>
#include <LocalBoost/boost/type_traits/has_equal_to.hpp>
#include <LocalBoost/boost/type_traits/has_greater.hpp>
#include <LocalBoost/boost/type_traits/has_greater_equal.hpp>
#include <LocalBoost/boost/type_traits/has_left_shift.hpp>
#include <LocalBoost/boost/type_traits/has_left_shift_assign.hpp>
#include <LocalBoost/boost/type_traits/has_less.hpp>
#include <LocalBoost/boost/type_traits/has_less_equal.hpp>
#include <LocalBoost/boost/type_traits/has_logical_and.hpp>
#include <LocalBoost/boost/type_traits/has_logical_not.hpp>
#include <LocalBoost/boost/type_traits/has_logical_or.hpp>
#include <LocalBoost/boost/type_traits/has_minus.hpp>
#include <LocalBoost/boost/type_traits/has_minus_assign.hpp>
#include <LocalBoost/boost/type_traits/has_modulus.hpp>
#include <LocalBoost/boost/type_traits/has_modulus_assign.hpp>
#include <LocalBoost/boost/type_traits/has_multiplies.hpp>
#include <LocalBoost/boost/type_traits/has_multiplies_assign.hpp>
#include <LocalBoost/boost/type_traits/has_negate.hpp>
#if !defined(BOOST_BORLANDC) && (!(defined(__CUDACC__) && (__CUDACC_VER_MAJOR__ < 11)) || defined(__CUDA__))
#include <LocalBoost/boost/type_traits/has_new_operator.hpp>
#endif
#include <LocalBoost/boost/type_traits/has_not_equal_to.hpp>
#include <LocalBoost/boost/type_traits/has_nothrow_assign.hpp>
#include <LocalBoost/boost/type_traits/has_nothrow_constructor.hpp>
#include <LocalBoost/boost/type_traits/has_nothrow_copy.hpp>
#include <LocalBoost/boost/type_traits/has_nothrow_destructor.hpp>
#include <LocalBoost/boost/type_traits/has_plus.hpp>
#include <LocalBoost/boost/type_traits/has_plus_assign.hpp>
#include <LocalBoost/boost/type_traits/has_post_decrement.hpp>
#include <LocalBoost/boost/type_traits/has_post_increment.hpp>
#include <LocalBoost/boost/type_traits/has_pre_decrement.hpp>
#include <LocalBoost/boost/type_traits/has_pre_increment.hpp>
#include <LocalBoost/boost/type_traits/has_right_shift.hpp>
#include <LocalBoost/boost/type_traits/has_right_shift_assign.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_assign.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_constructor.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_copy.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_destructor.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_move_assign.hpp>
#include <LocalBoost/boost/type_traits/has_trivial_move_constructor.hpp>
#include <LocalBoost/boost/type_traits/has_unary_minus.hpp>
#include <LocalBoost/boost/type_traits/has_unary_plus.hpp>
#include <LocalBoost/boost/type_traits/has_virtual_destructor.hpp>

#include <LocalBoost/boost/type_traits/integral_constant.hpp>

#include <LocalBoost/boost/type_traits/is_abstract.hpp>
#include <LocalBoost/boost/type_traits/is_arithmetic.hpp>
#include <LocalBoost/boost/type_traits/is_array.hpp>
#include <LocalBoost/boost/type_traits/is_assignable.hpp>
#include <LocalBoost/boost/type_traits/is_base_and_derived.hpp>
#include <LocalBoost/boost/type_traits/is_base_of.hpp>
#include <LocalBoost/boost/type_traits/is_class.hpp>
#include <LocalBoost/boost/type_traits/is_complex.hpp>
#include <LocalBoost/boost/type_traits/is_compound.hpp>
#include <LocalBoost/boost/type_traits/is_const.hpp>
#include <LocalBoost/boost/type_traits/is_constructible.hpp>
#include <LocalBoost/boost/type_traits/is_convertible.hpp>
#include <LocalBoost/boost/type_traits/is_copy_assignable.hpp>
#include <LocalBoost/boost/type_traits/is_copy_constructible.hpp>
#include <LocalBoost/boost/type_traits/is_default_constructible.hpp>
#include <LocalBoost/boost/type_traits/is_destructible.hpp>
#include <LocalBoost/boost/type_traits/is_empty.hpp>
#include <LocalBoost/boost/type_traits/is_enum.hpp>
#include <LocalBoost/boost/type_traits/is_final.hpp>
#include <LocalBoost/boost/type_traits/is_float.hpp>
#include <LocalBoost/boost/type_traits/is_floating_point.hpp>
#include <LocalBoost/boost/type_traits/is_function.hpp>
#include <LocalBoost/boost/type_traits/is_fundamental.hpp>
#include <LocalBoost/boost/type_traits/is_integral.hpp>
#include <LocalBoost/boost/type_traits/is_list_constructible.hpp>
#include <LocalBoost/boost/type_traits/is_lvalue_reference.hpp>
#include <LocalBoost/boost/type_traits/is_member_function_pointer.hpp>
#include <LocalBoost/boost/type_traits/is_member_object_pointer.hpp>
#include <LocalBoost/boost/type_traits/is_member_pointer.hpp>
#include <LocalBoost/boost/type_traits/is_nothrow_move_assignable.hpp>
#include <LocalBoost/boost/type_traits/is_nothrow_move_constructible.hpp>
#include <LocalBoost/boost/type_traits/is_nothrow_swappable.hpp>
#include <LocalBoost/boost/type_traits/is_object.hpp>
#include <LocalBoost/boost/type_traits/is_pod.hpp>
#include <LocalBoost/boost/type_traits/is_pointer.hpp>
#include <LocalBoost/boost/type_traits/is_polymorphic.hpp>
#include <LocalBoost/boost/type_traits/is_reference.hpp>
#include <LocalBoost/boost/type_traits/is_rvalue_reference.hpp>
#include <LocalBoost/boost/type_traits/is_same.hpp>
#include <LocalBoost/boost/type_traits/is_scalar.hpp>
#include <LocalBoost/boost/type_traits/is_scoped_enum.hpp>
#include <LocalBoost/boost/type_traits/is_signed.hpp>
#include <LocalBoost/boost/type_traits/is_stateless.hpp>
#include <LocalBoost/boost/type_traits/is_trivially_copyable.hpp>
#include <LocalBoost/boost/type_traits/is_union.hpp>
#include <LocalBoost/boost/type_traits/is_unscoped_enum.hpp>
#include <LocalBoost/boost/type_traits/is_unsigned.hpp>
#include <LocalBoost/boost/type_traits/is_virtual_base_of.hpp>
#include <LocalBoost/boost/type_traits/is_void.hpp>
#include <LocalBoost/boost/type_traits/is_volatile.hpp>
#include <LocalBoost/boost/type_traits/make_signed.hpp>
#include <LocalBoost/boost/type_traits/make_unsigned.hpp>
#include <LocalBoost/boost/type_traits/make_void.hpp>
#include <LocalBoost/boost/type_traits/negation.hpp>
#include <LocalBoost/boost/type_traits/rank.hpp>
#include <LocalBoost/boost/type_traits/remove_all_extents.hpp>
#include <LocalBoost/boost/type_traits/remove_bounds.hpp>
#include <LocalBoost/boost/type_traits/remove_const.hpp>
#include <LocalBoost/boost/type_traits/remove_cv.hpp>
#include <LocalBoost/boost/type_traits/remove_cv_ref.hpp>
#include <LocalBoost/boost/type_traits/remove_extent.hpp>
#include <LocalBoost/boost/type_traits/remove_pointer.hpp>
#include <LocalBoost/boost/type_traits/remove_reference.hpp>
#include <LocalBoost/boost/type_traits/remove_volatile.hpp>
#include <LocalBoost/boost/type_traits/type_identity.hpp>
#include <LocalBoost/boost/type_traits/type_with_alignment.hpp>

#if !(defined(__sgi) && defined(__EDG_VERSION__) && (__EDG_VERSION__ == 238))
#include <LocalBoost/boost/type_traits/integral_promotion.hpp>
#include <LocalBoost/boost/type_traits/promote.hpp>
#endif

#endif // BOOST_TYPE_TRAITS_HPP
