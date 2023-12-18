// (C) Copyright 2005 Matthias Troyer

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Matthias Troyer

#ifndef BOOST_MPI_DETAIL_MPI_DATATYPE_OARCHIVE_HPP
#define BOOST_MPI_DETAIL_MPI_DATATYPE_OARCHIVE_HPP

#include <LocalBoost/boost/type_traits/is_enum.hpp>
#include <LocalBoost/boost/mpl/bool.hpp>
#include <LocalBoost/boost/archive/detail/oserializer.hpp>
#include <LocalBoost/boost/archive/detail/auto_link_archive.hpp>
#include <LocalBoost/boost/archive/basic_archive.hpp>
#include <LocalBoost/boost/mpi/detail/ignore_skeleton_oarchive.hpp>
#include <LocalBoost/boost/mpi/detail/mpi_datatype_primitive.hpp>
#include <LocalBoost/boost/mpi/datatype_fwd.hpp>
#include <LocalBoost/boost/mpl/assert.hpp>
#include <LocalBoost/boost/static_assert.hpp>
#include <LocalBoost/boost/integer.hpp>
#include <LocalBoost/boost/archive/detail/register_archive.hpp>

namespace boost { namespace mpi { namespace detail {


// an archive wrapper that stores only the data members but not the
// special types defined by the serialization library
// to define the data skeletons (classes, pointers, container sizes, ...)

class mpi_datatype_oarchive
  : public mpi_datatype_primitive,
    public ignore_skeleton_oarchive<mpi_datatype_oarchive>
{
public:
    template <class T>
    mpi_datatype_oarchive(const T& x)
         :  mpi_datatype_primitive(&x) // register address
        {
          BOOST_MPL_ASSERT((is_mpi_datatype<T>));
          *this << x;                   // serialize the object
        }

    template<class T>
    void save_override(T const& t)
    {
      save_enum(t,boost::is_enum<T>());
    }

    template<class T>
    void save_enum(T const& t, mpl::false_)
    {
      ignore_skeleton_oarchive<mpi_datatype_oarchive>::save_override(t);
    }

    template<class T>
    void save_enum(T const& t, mpl::true_)
    {
      // select the right sized integer for the enum
      typedef typename boost::uint_t<8*sizeof(T)>::least int_type;
      BOOST_STATIC_ASSERT((sizeof(T)==sizeof(int_type)));
      this->save(*reinterpret_cast<int_type const*>(&t));
    }

};

} } } // end namespace boost::mpi::detail

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::mpi::detail::mpi_datatype_oarchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::mpi::detail::ignore_skeleton_oarchive<boost::mpi::detail::mpi_datatype_oarchive>)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::mpi::detail::mpi_datatype_oarchive)

#endif // BOOST_MPI_DETAIL_MPI_DATATYPE_OARCHIVE_HPP
