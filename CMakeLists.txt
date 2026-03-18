cmake_minimum_required(VERSION 4.1)
project(snake)

set(CMAKE_CXX_STANDARD 23)

include_directories(libraries/glfw/include)
include_directories(libraries/glad/include)
include_directories(libraries/glm)

add_executable(snake main.cpp libraries/glad/src/glad.c
        shader.h
        sprite_renderer.h)

target_link_libraries(snake
        "${CMAKE_SOURCE_DIR}/libraries/glfw/libglfw3.a"
        opengl32
)


