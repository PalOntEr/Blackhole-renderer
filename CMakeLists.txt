cmake_minimum_required(VERSION 3.10)
project(BlackholeRenderer)

set(CMAKE_CXX_STANDARD 17)

# Find packages (installed via vcpkg)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(renderer renderer/main.cpp)
target_link_libraries(renderer 
    glfw 
    glad::glad 
    OpenGL::GL
)