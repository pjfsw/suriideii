cmake_minimum_required(VERSION 3.10)

project(suriideii)

set(CMAKE_C_STANDARD 11)
set(OpenGL_GL_PREFERENCE GLVND)
add_compile_options(-Wall -Wextra -pedantic -Werror)

include(FindPkgConfig)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

#pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

#find_package(SDL2 REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

add_executable(suriideii
    src/main.c
    src/shader_program.c
    src/algebra.c
)
#configure_file(resources/font.png ./font.png COPYONLY)
#configure_file(resources/pixla.png ./pixla.png COPYONLY)

configure_file(resources/shader.vs ./shader.vs COPYONLY)
configure_file(resources/shader.fs ./shader.fs COPYONLY)

target_link_libraries(suriideii ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

