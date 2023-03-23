#ifndef BOOST_DESCRIBE_DETAIL_PP_FOR_EACH_HPP_INCLUDED
#define BOOST_DESCRIBE_DETAIL_PP_FOR_EACH_HPP_INCLUDED

// Copyright 2020 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <LocalBoost/boost/describe/detail/pp_utilities.hpp>

#define BOOST_DESCRIBE_PP_FOR_EACH_0(F, a)
#define BOOST_DESCRIBE_PP_FOR_EACH_1(F, a, x) BOOST_DESCRIBE_PP_CALL(F, a, x)
#define BOOST_DESCRIBE_PP_FOR_EACH_2(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_1(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_3(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_2(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_4(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_3(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_5(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_4(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_6(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_5(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_7(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_6(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_8(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_7(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_9(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_8(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_10(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_9(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_11(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_10(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_12(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_11(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_13(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_12(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_14(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_13(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_15(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_14(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_16(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_15(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_17(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_16(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_18(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_17(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_19(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_18(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_20(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_19(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_21(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_20(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_22(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_21(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_23(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_22(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_24(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_23(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_25(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_24(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_26(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_25(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_27(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_26(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_28(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_27(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_29(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_28(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_30(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_29(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_31(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_30(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_32(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_31(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_33(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_32(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_34(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_33(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_35(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_34(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_36(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_35(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_37(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_36(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_38(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_37(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_39(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_38(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_40(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_39(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_41(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_40(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_42(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_41(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_43(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_42(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_44(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_43(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_45(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_44(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_46(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_45(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_47(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_46(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_48(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_47(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_49(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_48(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_50(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_49(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_51(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_50(F, a, __VA_ARGS__))
#define BOOST_DESCRIBE_PP_FOR_EACH_52(F, a, x, ...) BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_CALL(F, a, x) BOOST_DESCRIBE_PP_FOR_EACH_51(F, a, __VA_ARGS__))

#define BOOST_DESCRIBE_PP_FE_EXTRACT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, V, ...) V

#define BOOST_DESCRIBE_PP_FOR_EACH(F, ...) \
    BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_EXPAND(BOOST_DESCRIBE_PP_FE_EXTRACT(__VA_ARGS__, \
    BOOST_DESCRIBE_PP_FOR_EACH_52, \
    BOOST_DESCRIBE_PP_FOR_EACH_51, \
    BOOST_DESCRIBE_PP_FOR_EACH_50, \
    BOOST_DESCRIBE_PP_FOR_EACH_49, \
    BOOST_DESCRIBE_PP_FOR_EACH_48, \
    BOOST_DESCRIBE_PP_FOR_EACH_47, \
    BOOST_DESCRIBE_PP_FOR_EACH_46, \
    BOOST_DESCRIBE_PP_FOR_EACH_45, \
    BOOST_DESCRIBE_PP_FOR_EACH_44, \
    BOOST_DESCRIBE_PP_FOR_EACH_43, \
    BOOST_DESCRIBE_PP_FOR_EACH_42, \
    BOOST_DESCRIBE_PP_FOR_EACH_41, \
    BOOST_DESCRIBE_PP_FOR_EACH_40, \
    BOOST_DESCRIBE_PP_FOR_EACH_39, \
    BOOST_DESCRIBE_PP_FOR_EACH_38, \
    BOOST_DESCRIBE_PP_FOR_EACH_37, \
    BOOST_DESCRIBE_PP_FOR_EACH_36, \
    BOOST_DESCRIBE_PP_FOR_EACH_35, \
    BOOST_DESCRIBE_PP_FOR_EACH_34, \
    BOOST_DESCRIBE_PP_FOR_EACH_33, \
    BOOST_DESCRIBE_PP_FOR_EACH_32, \
    BOOST_DESCRIBE_PP_FOR_EACH_31, \
    BOOST_DESCRIBE_PP_FOR_EACH_30, \
    BOOST_DESCRIBE_PP_FOR_EACH_29, \
    BOOST_DESCRIBE_PP_FOR_EACH_28, \
    BOOST_DESCRIBE_PP_FOR_EACH_27, \
    BOOST_DESCRIBE_PP_FOR_EACH_26, \
    BOOST_DESCRIBE_PP_FOR_EACH_25, \
    BOOST_DESCRIBE_PP_FOR_EACH_24, \
    BOOST_DESCRIBE_PP_FOR_EACH_23, \
    BOOST_DESCRIBE_PP_FOR_EACH_22, \
    BOOST_DESCRIBE_PP_FOR_EACH_21, \
    BOOST_DESCRIBE_PP_FOR_EACH_20, \
    BOOST_DESCRIBE_PP_FOR_EACH_19, \
    BOOST_DESCRIBE_PP_FOR_EACH_18, \
    BOOST_DESCRIBE_PP_FOR_EACH_17, \
    BOOST_DESCRIBE_PP_FOR_EACH_16, \
    BOOST_DESCRIBE_PP_FOR_EACH_15, \
    BOOST_DESCRIBE_PP_FOR_EACH_14, \
    BOOST_DESCRIBE_PP_FOR_EACH_13, \
    BOOST_DESCRIBE_PP_FOR_EACH_12, \
    BOOST_DESCRIBE_PP_FOR_EACH_11, \
    BOOST_DESCRIBE_PP_FOR_EACH_10, \
    BOOST_DESCRIBE_PP_FOR_EACH_9, \
    BOOST_DESCRIBE_PP_FOR_EACH_8, \
    BOOST_DESCRIBE_PP_FOR_EACH_7, \
    BOOST_DESCRIBE_PP_FOR_EACH_6, \
    BOOST_DESCRIBE_PP_FOR_EACH_5, \
    BOOST_DESCRIBE_PP_FOR_EACH_4, \
    BOOST_DESCRIBE_PP_FOR_EACH_3, \
    BOOST_DESCRIBE_PP_FOR_EACH_2, \
    BOOST_DESCRIBE_PP_FOR_EACH_1, \
    BOOST_DESCRIBE_PP_FOR_EACH_0))(F, __VA_ARGS__))

#endif // #ifndef BOOST_DESCRIBE_DETAIL_PP_FOR_EACH_HPP_INCLUDED
