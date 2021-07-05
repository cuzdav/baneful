enable_testing()

add_executable(testphase1
  TestColor.cpp
  TestGraph.cpp
  TestTransforms.cpp
  TestVerticies.cpp
)
target_link_libraries(testphase1 phase1 gtest_main)

include(GoogleTest)
gtest_discover_tests(testphase1)
