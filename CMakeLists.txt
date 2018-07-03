project(marching_cubes)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

find_package(OpenGL)
find_package(GLUT)

add_executable(MarchingCubes main.cpp scripts/data.cpp scripts/rimls.cpp)
target_link_libraries(
    MarchingCubes
    ${OPENGL_gl_LIBRARY}
    ${GLUT_LIBRARIES} )

