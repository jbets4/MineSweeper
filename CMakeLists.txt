cmake_minimum_required(VERSION 3.14)
project(Project3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp
        board.cpp
        Tile.h
        board.h
        Tile.cpp
        TextureManager.h
        TextureManager.cpp)

add_executable(Project3 ${SOURCE_FILES})

# Update this path based on the output from brew --prefix
set(SFML_DIR "/opt/homebrew/opt/sfml@2/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(Project3 sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
