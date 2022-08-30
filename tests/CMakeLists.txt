# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.1)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(testlib testlib.cpp)
add_executable(testannlib test_ann.cpp)
add_executable(testprimitivelib test_primitives.cpp)

# I'm using C++17 in the test
target_compile_features(testlib PRIVATE cxx_std_17)
target_compile_features(testannlib PRIVATE cxx_std_17)
target_compile_features(testprimitivelib PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
#target_link_libraries(testlib PRIVATE modern_library Catch2::Catch2)
target_link_libraries(testlib PRIVATE Catch2::Catch2)
target_link_libraries(testannlib PRIVATE ann Catch2::Catch2)
target_link_libraries(testprimitivelib PRIVATE ray Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibtest COMMAND testlib) # Command can be a target
add_test(NAME testannlibtest COMMAND testannlib)
add_test(NAME testprimitivelib COMMAND testprimitivelib )
