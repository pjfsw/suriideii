cmake_minimum_required(VERSION 3.10)

project(suriideii)

set(CMAKE_C_STANDARD 11)
set(OpenGL_GL_PREFERENCE GLVND)
add_compile_options(-Wall -Wextra -pedantic -Werror)

include(FindPkgConfig)

pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_executable(suriideii
    src/main.c
    src/shader_program.c
    src/algebra.c
    src/mesh.c
    src/transform.c
    src/camera.c
    src/texture.c
    src/meshloader.c
    src/object.c
)

configure_file(resources/shader.vs ./shader.vs COPYONLY)
configure_file(resources/shader.fs ./shader.fs COPYONLY)
#configure_file(resources/texture.jpg ./texture.jpg COPYONLY)
#configure_file(resources/skull.obj ./skull.obj COPYONLY)
#configure_file(resources/skull.mtl ./skull.mtl COPYONLY)
#configure_file(resources/skull.jpg ./skull.jpg COPYONLY)

add_custom_command(TARGET suriideii POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources .)

target_link_libraries(suriideii ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} m )

