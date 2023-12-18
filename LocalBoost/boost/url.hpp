//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_HPP
#define BOOST_URL_HPP

#include <LocalBoost/boost/url/grammar.hpp>

#include <LocalBoost/boost/url/authority_view.hpp>
#include <LocalBoost/boost/url/decode_view.hpp>
#include <LocalBoost/boost/url/encode.hpp>
#include <LocalBoost/boost/url/encoding_opts.hpp>
#include <LocalBoost/boost/url/error.hpp>
#include <LocalBoost/boost/url/error_types.hpp>
#include <LocalBoost/boost/url/host_type.hpp>
#include <LocalBoost/boost/url/ignore_case.hpp>
#include <LocalBoost/boost/url/ipv4_address.hpp>
#include <LocalBoost/boost/url/ipv6_address.hpp>
#include <LocalBoost/boost/url/optional.hpp>
#include <LocalBoost/boost/url/param.hpp>
#include <LocalBoost/boost/url/params_base.hpp>
#include <LocalBoost/boost/url/params_encoded_base.hpp>
#include <LocalBoost/boost/url/params_encoded_ref.hpp>
#include <LocalBoost/boost/url/params_encoded_view.hpp>
#include <LocalBoost/boost/url/params_ref.hpp>
#include <LocalBoost/boost/url/params_view.hpp>
#include <LocalBoost/boost/url/parse.hpp>
#include <LocalBoost/boost/url/parse_path.hpp>
#include <LocalBoost/boost/url/parse_query.hpp>
#include <LocalBoost/boost/url/pct_string_view.hpp>
#include <LocalBoost/boost/url/scheme.hpp>
#include <LocalBoost/boost/url/segments_base.hpp>
#include <LocalBoost/boost/url/segments_encoded_base.hpp>
#include <LocalBoost/boost/url/segments_encoded_ref.hpp>
#include <LocalBoost/boost/url/segments_encoded_view.hpp>
#include <LocalBoost/boost/url/segments_ref.hpp>
#include <LocalBoost/boost/url/segments_view.hpp>
#include <LocalBoost/boost/url/static_url.hpp>
#include <LocalBoost/boost/url/string_view.hpp>
#include <LocalBoost/boost/url/url.hpp>
#include <LocalBoost/boost/url/url_base.hpp>
#include <LocalBoost/boost/url/url_view.hpp>
#include <LocalBoost/boost/url/url_view_base.hpp>
#include <LocalBoost/boost/url/urls.hpp>
#include <LocalBoost/boost/url/variant.hpp>

#include <LocalBoost/boost/url/rfc/absolute_uri_rule.hpp>
#include <LocalBoost/boost/url/rfc/authority_rule.hpp>
#include <LocalBoost/boost/url/rfc/gen_delim_chars.hpp>
#include <LocalBoost/boost/url/rfc/ipv4_address_rule.hpp>
#include <LocalBoost/boost/url/rfc/ipv6_address_rule.hpp>
#include <LocalBoost/boost/url/rfc/origin_form_rule.hpp>
#include <LocalBoost/boost/url/rfc/pchars.hpp>
#include <LocalBoost/boost/url/rfc/pct_encoded_rule.hpp>
#include <LocalBoost/boost/url/rfc/query_rule.hpp>
#include <LocalBoost/boost/url/rfc/relative_ref_rule.hpp>
#include <LocalBoost/boost/url/rfc/reserved_chars.hpp>
#include <LocalBoost/boost/url/rfc/sub_delim_chars.hpp>
#include <LocalBoost/boost/url/rfc/unreserved_chars.hpp>
#include <LocalBoost/boost/url/rfc/uri_rule.hpp>
#include <LocalBoost/boost/url/rfc/uri_reference_rule.hpp>

#endif
