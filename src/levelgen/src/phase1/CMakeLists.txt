set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")

add_subdirectory(test)

add_library (phase1
    Graph.cpp
    GraphLoader.cpp
    Verticies.cpp
)

target_link_libraries(phase1 LINK_PUBLIC boost_json)

target_include_directories (phase1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
