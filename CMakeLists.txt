# Works with 3.11 and tested through 3.18
cmake_minimum_required(VERSION 3.10...3.18)

# Project name and a few useful settings. Other commands can pick up the results
project(
  Raytracer
  VERSION 0.1
  DESCRIPTION "A simple raytrace algorithm"
  LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  #include(CTest)

  # Docs only available if this is the main app
  #find_package(Doxygen)
  #if(Doxygen_FOUND)
  #  add_subdirectory(docs)
  #else()
  #  message(STATUS "Doxygen not found, not building docs")
  #endif()
endif()

# Accumulator library
# This is header only, so could be replaced with git submodules or FetchContent
#find_package(Boost REQUIRED)
# Adds Boost::boost

# Formatting library
#FetchContent_Declare(
#  fmtlib
#  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#  GIT_TAG 5.3.0)
#FetchContent_MakeAvailable(fmtlib)
# Adds fmt::fmt

# The compiled library code is here
add_subdirectory(src/ann)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

# installs examples directory to /usr/examples
install(DIRECTORY 
        "${PROJECT_SOURCE_DIR}/examples/" 
        DESTINATION "examples")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sean")

include(CPack)

# to use this run
# make package
# sudo dpkg -i Raytrace*.deb
# examples in /usr/examples
# installed in /usr/bin/raytrace