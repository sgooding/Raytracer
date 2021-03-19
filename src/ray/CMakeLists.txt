# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/ray/*.h")

# Make an automatic library - will be static or dynamic based on user setting
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")
add_library(ray ${SOURCE_LIST} ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(ray PUBLIC ../../include)

# This depends on (header only) boost
target_link_libraries(ray PRIVATE)

# All users of this library will need at least C++11
target_compile_features(ray PUBLIC)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

