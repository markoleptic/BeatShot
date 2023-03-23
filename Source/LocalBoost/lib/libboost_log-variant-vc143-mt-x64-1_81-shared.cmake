# Generated by Boost 1.81.0

# address-model=64

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "64 bit, need 32")
  return()
endif()

# layout=versioned

# toolset=vc143

if(Boost_COMPILER)
  if(NOT "vc143" IN_LIST Boost_COMPILER AND NOT "-vc143" IN_LIST Boost_COMPILER)
    _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "vc143, Boost_COMPILER=${Boost_COMPILER}")
    return()
  endif()
else()
  if(BOOST_DETECTED_TOOLSET AND NOT BOOST_DETECTED_TOOLSET STREQUAL "vc143")
    _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "vc143, detected ${BOOST_DETECTED_TOOLSET}, set Boost_COMPILER to override")
    return()
  endif()
endif()

# link=shared

if(DEFINED Boost_USE_STATIC_LIBS)
  if(Boost_USE_STATIC_LIBS)
    _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "shared, Boost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}")
    return()
  endif()
else()
  if(WIN32 AND NOT _BOOST_SINGLE_VARIANT)
    _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "shared, default on Windows is static, set Boost_USE_STATIC_LIBS=OFF to override")
    return()
  endif()
endif()

# runtime-link=shared

if(Boost_USE_STATIC_RUNTIME)
  _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "shared runtime, Boost_USE_STATIC_RUNTIME=${Boost_USE_STATIC_RUNTIME}")
  return()
endif()

# runtime-debugging=off

if(Boost_USE_DEBUG_RUNTIME)
  _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "release runtime, Boost_USE_DEBUG_RUNTIME=${Boost_USE_DEBUG_RUNTIME}")
  return()
endif()

# threading=multi

if(DEFINED Boost_USE_MULTITHREADED AND NOT Boost_USE_MULTITHREADED)
  _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "multithreaded, Boost_USE_MULTITHREADED=${Boost_USE_MULTITHREADED}")
  return()
endif()

# variant=release

if(NOT "${Boost_USE_RELEASE_LIBS}" STREQUAL "" AND NOT Boost_USE_RELEASE_LIBS)
  _BOOST_SKIPPED("boost_log-vc143-mt-x64-1_81.lib" "release, Boost_USE_RELEASE_LIBS=${Boost_USE_RELEASE_LIBS}")
  return()
endif()

if(Boost_VERBOSE OR Boost_DEBUG)
  message(STATUS "  [x] boost_log-vc143-mt-x64-1_81.lib")
endif()

# Create imported target Boost::log

if(NOT TARGET Boost::log)
  add_library(Boost::log SHARED IMPORTED)

  set_target_properties(Boost::log PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_BOOST_INCLUDEDIR}"
    INTERFACE_COMPILE_DEFINITIONS "BOOST_LOG_NO_LIB"
  )
endif()

# Target file name: boost_log-vc143-mt-x64-1_81.lib

get_target_property(__boost_imploc Boost::log IMPORTED_IMPLIB_RELEASE)
if(__boost_imploc)
  message(SEND_ERROR "Target Boost::log already has an imported location '${__boost_imploc}', which is being overwritten with '${_BOOST_LIBDIR}/boost_log-vc143-mt-x64-1_81.lib'")
endif()
unset(__boost_imploc)

set_property(TARGET Boost::log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

set_target_properties(Boost::log PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE CXX
  IMPORTED_IMPLIB_RELEASE "${_BOOST_LIBDIR}/boost_log-vc143-mt-x64-1_81.lib"
  )

set_target_properties(Boost::log PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_BOOST_LIBDIR}/boost_log-vc143-mt-x64-1_81.dll"
  )

set_target_properties(Boost::log PROPERTIES
  MAP_IMPORTED_CONFIG_MINSIZEREL Release
  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )

set_property(TARGET Boost::log APPEND
  PROPERTY INTERFACE_COMPILE_DEFINITIONS "BOOST_LOG_DYN_LINK"
  )

list(APPEND _BOOST_LOG_DEPS atomic chrono filesystem regex thread headers)
