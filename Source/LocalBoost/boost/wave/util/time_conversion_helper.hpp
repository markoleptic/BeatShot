/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library

    http://www.boost.org/

    Copyright (c) 2001-2012 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_TIME_CONVERSION_HELPER_HPP_DA97E389_1797_43BA_82AE_B071064B3EF4_INCLUDED)
#define BOOST_TIME_CONVERSION_HELPER_HPP_DA97E389_1797_43BA_82AE_B071064B3EF4_INCLUDED

#include <ctime>
#include <cstring>

#include <LocalBoost/boost/config.hpp>
#include <LocalBoost/boost/spirit/include/classic_core.hpp>
#include <LocalBoost/boost/spirit/include/classic_symbols.hpp>
#include <LocalBoost/boost/spirit/include/classic_assign_actor.hpp>
#include <LocalBoost/boost/spirit/include/classic_push_back_actor.hpp>

#if !defined(spirit_append_actor)
#define spirit_append_actor(actor) boost::spirit::classic::push_back_a(actor)
#define spirit_assign_actor(actor) boost::spirit::classic::assign_a(actor)
#endif // !defined(spirit_append_actor)

// this must occur after all of the includes and before any code appears
#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_PREFIX
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace util {

namespace time_conversion {

    using namespace std;    // some systems have std::tm etc. in namespace std

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CPP_TIME_CONVERSION \
    (BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_TIME_CONVERSION) \
    /**/

///////////////////////////////////////////////////////////////////////////////
//  Grammar for parsing a date/time string generated by the C++ compiler from
//  __DATE__ and __TIME__
    class time_conversion_grammar :
        public boost::spirit::classic::grammar<time_conversion_grammar>
    {
    public:
        time_conversion_grammar() : fYearIsCorrected(false)
        {
            using namespace std;        // some systems have memset in std
            memset (&time_stamp, 0, sizeof(tm));
            BOOST_SPIRIT_DEBUG_TRACE_RULE_NAME(*this, "time_conversion_grammar",
                TRACE_CPP_TIME_CONVERSION);
        }

        template <typename ScannerT>
        struct definition {

            definition(time_conversion_grammar const &self)
            {
                using boost::spirit::classic::int_p;
                using boost::spirit::classic::add;

            char const *m[] = {
                    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                };

                for (int i = 0; i < 12; ++i)
                    add (month, m[i], i);

                time_rule       // expected format is 'Dec 29 2001 11:23:59'
                    =   month[spirit_assign_actor(self.time_stamp.tm_mon)]
                    >>  int_p[spirit_assign_actor(self.time_stamp.tm_mday)]
                    >>  int_p[spirit_assign_actor(self.time_stamp.tm_year)]
                    >>  int_p[spirit_assign_actor(self.time_stamp.tm_hour)] >> ':'
                    >>  int_p[spirit_assign_actor(self.time_stamp.tm_min)] >> ':'
                    >>  int_p[spirit_assign_actor(self.time_stamp.tm_sec)]
                    ;

                BOOST_SPIRIT_DEBUG_TRACE_RULE(time_rule, TRACE_CPP_TIME_CONVERSION);
            }

            boost::spirit::classic::rule<ScannerT> time_rule;
            boost::spirit::classic::symbols<> month;

            boost::spirit::classic::rule<ScannerT> const&
            start() const { return time_rule; }
        };

        void correct_year()
        {
            if (!fYearIsCorrected) {
                time_stamp.tm_year -= 1900;
                fYearIsCorrected = true;
            }
        }

        mutable tm time_stamp;
        bool fYearIsCorrected;
    };

///////////////////////////////////////////////////////////////////////////////
// calculate the time of the compilation as a std::time_t to ensure correctness
// of the saved dfa table
    class time_conversion_helper
    {
    public:
        time_conversion_helper(char const *act_time) : compile_time(0)
        {
            using namespace boost::spirit::classic;

        time_conversion_grammar g;
        parse_info<> pi = parse (act_time, g, space_p);

            if (pi.hit) {
                g.correct_year();
                compile_time = mktime(&g.time_stamp);
            }
            BOOST_ASSERT(0 != compile_time);
        }

        time_t get_time() const { return compile_time; }

    private:
        time_t compile_time;
    };

///////////////////////////////////////////////////////////////////////////////
#undef TRACE_CPP_TIME_CONVERSION
}   // namespace time_conversion

// import time_conversion into the boost::wave::util namespace
using namespace time_conversion;

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

// the suffix header occurs after all of the code
#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_SUFFIX
#endif

#endif // !defined(BOOST_TIME_CONVERSION_HELPER_HPP_DA97E389_1797_43BA_82AE_B071064B3EF4_INCLUDED)
