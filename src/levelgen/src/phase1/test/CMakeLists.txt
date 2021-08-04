enable_testing()

add_executable(testphase1
  algotest.cpp
  TestAdjacencyMatrix.cpp
  TestColor.cpp
  TestGraph.cpp
  TestGraphCreator.cpp
  TestTransforms.cpp
  TestVertex.cpp
  TestVertices.cpp
)
target_link_libraries(testphase1 phase1 gtest_main)

include(GoogleTest)
gtest_discover_tests(testphase1)
