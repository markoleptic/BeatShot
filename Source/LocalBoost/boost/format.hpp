// ----------------------------------------------------------------------------
// format.hpp :  primary header
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/format for library home page


// ----------------------------------------------------------------------------

#ifndef BOOST_FORMAT_HPP
#define BOOST_FORMAT_HPP

#include <vector>
#include <string>
#include <LocalBoost/boost/detail/workaround.hpp>
#include <LocalBoost/boost/config.hpp>

#ifndef BOOST_NO_STD_LOCALE
#include <locale>
#endif

// ***   Compatibility framework
#include <LocalBoost/boost/format/detail/compat_workarounds.hpp>

#ifdef BOOST_NO_LOCALE_ISDIGIT
#include <cctype>  // we'll use the non-locale  <cctype>'s std::isdigit(int)
#endif

// ****  Forward declarations ----------------------------------
#include <LocalBoost/boost/format/format_fwd.hpp>     // basic_format<Ch,Tr>, and other frontends
#include <LocalBoost/boost/format/internals_fwd.hpp>  // misc forward declarations for internal use

// ****  Auxiliary structs (stream_format_state<Ch,Tr> , and format_item<Ch,Tr> )
#include <LocalBoost/boost/format/internals.hpp>    

// ****  Format  class  interface --------------------------------
#include <LocalBoost/boost/format/format_class.hpp>

// **** Exceptions -----------------------------------------------
#include <LocalBoost/boost/format/exceptions.hpp>

// **** Implementation -------------------------------------------
#include <LocalBoost/boost/format/format_implementation.hpp>   // member functions
#include <LocalBoost/boost/format/group.hpp>                   // class for grouping arguments
#include <LocalBoost/boost/format/feed_args.hpp>               // argument-feeding functions
#include <LocalBoost/boost/format/parsing.hpp>                 // format-string parsing (member-)functions

// **** Implementation of the free functions ----------------------
#include <LocalBoost/boost/format/free_funcs.hpp>


// *** Undefine 'local' macros :
#include <LocalBoost/boost/format/detail/unset_macros.hpp>

#endif // BOOST_FORMAT_HPP
