add_library (LevelGenLib levelgen.cpp)

target_include_directories (LevelGenLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (LevelGenLib LINK_PUBLIC
    phase1
)

add_executable (levelgen main.cpp)
target_link_libraries (levelgen LINK_PUBLIC LevelGenLib)

add_subdirectory(phase1)
