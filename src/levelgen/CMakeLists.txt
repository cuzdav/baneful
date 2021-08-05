cmake_minimum_required (VERSION 3.20.3)
project (LEVELGEN)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# for clangd and other clang tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  UPDATE_DISCONNECTED 0,
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)

option(BOOST_JSON_STANDALONE "Build boost::json as a standalone library" ON)
option(BOOST_JSON_BUILD_TESTS "Build boost::json tests" OFF)
option(BOOST_JSON_BUILD_FUZZERS "Build boost::json fuzzers" OFF)
option(BOOST_JSON_BUILD_EXAMPLES "Build boost::json examples" OFF)

FetchContent_Declare(
  boost_json
  UPDATE_DISCONNECTED 0,
  GIT_REPOSITORY https://github.com/boostorg/json
  GIT_TAG        
)

# google tests do not build cleanly.  Disable some warnings.
set_property(
    DIRECTORY ${googletest_SOURCE_DIR}
    APPEND
    PROPERTY COMPILE_OPTIONS -Wno-undef -Wno-maybe-uninitialized
)

FetchContent_Declare(
  fmt
  UPDATE_DISCONNECTED 0,
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG
)


FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(boost_json)
FetchContent_MakeAvailable(fmt)

add_subdirectory (src)
add_subdirectory (test)


