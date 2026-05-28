cmake_minimum_required(VERSION 3.10)
project(ya3dg C)

set(CMAKE_C_STANDARD 11)

# Add raylib include directory
include_directories(${CMAKE_SOURCE_DIR}/lib/raylib/src)

# Add your source files here
add_executable(ya3dg src/main.c src/game.c)
target_link_libraries(ya3dg raylib)

# Link against raylib
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/raylib)