// Copyright (C) 2006 Douglas Gregor <doug.gregor -at- gmail.com>.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Message Passing Interface

//  See www.boost.org/libs/mpi for documentation.

/** @file mpi.hpp
 *
 *  This file is a top-level convenience header that includes all of
 *  the Boost.MPI library headers. Users concerned about compile time
 *  may wish to include only specific headers from the Boost.MPI
 *  library.
 *
 */
#ifndef BOOST_MPI_HPP
#define BOOST_MPI_HPP

#include <LocalBoost/boost/mpi/allocator.hpp>
#include <LocalBoost/boost/mpi/collectives.hpp>
#include <LocalBoost/boost/mpi/communicator.hpp>
#include <LocalBoost/boost/mpi/datatype.hpp>
#include <LocalBoost/boost/mpi/environment.hpp>
#include <LocalBoost/boost/mpi/graph_communicator.hpp>
#include <LocalBoost/boost/mpi/group.hpp>
#include <LocalBoost/boost/mpi/intercommunicator.hpp>
#include <LocalBoost/boost/mpi/nonblocking.hpp>
#include <LocalBoost/boost/mpi/operations.hpp>
#include <LocalBoost/boost/mpi/skeleton_and_content.hpp>
#include <LocalBoost/boost/mpi/timer.hpp>

#endif // BOOST_MPI_HPP
