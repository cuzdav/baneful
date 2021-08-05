add_subdirectory(test)

add_library (phase1
    AdjacencyMatrix.cpp
    Graph.cpp
    GraphCreator.cpp
    GraphLoader.cpp
    Vertex.cpp
    Vertices.cpp
)

target_link_libraries(phase1 LINK_PUBLIC boost_json fmt::fmt)

target_include_directories (phase1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
