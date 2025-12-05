cmake_minimum_required(VERSION 3.16)

project(simple-graphing-calculator LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/dest/SGC
)

set(glad-sources
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/glad.c
)

set(stb_image-sources
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image/stb_image.cpp
)

set(imgui-sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_tables.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp
)

set(sgc-sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/error.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sgc_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(sources
    ${glad-sources}
    ${stb_image-sources}
    ${imgui-sources}
    ${sgc-sources}
)

if (WIN32)
    add_executable(sgc WIN32 ${sources} sgc.rc)
elseif (UNIX AND NOT APPLE)
    add_executable(sgc ${sources})
endif ()

target_include_directories(sgc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/mini
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(extern/glfw)

find_package(OpenGL REQUIRED)

if (WIN32)
    target_link_libraries(sgc glfw OpenGL::GL)
    target_link_options(sgc PRIVATE "-static")
elseif (UNIX AND NOT APPLE)
    target_link_libraries(sgc glfw OpenGL::GL dl pthread)
    target_link_options(sgc PRIVATE "-static-libgcc" "-static-libstdc++")
endif()

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wfloat-equal
    -Wformat=2
    -Wformat-security
    -Wnull-dereference
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Wmissing-declarations
    -Woverloaded-virtual
    -Wnon-virtual-dtor
    -Wunused
    -Wuninitialized
)

add_custom_command(TARGET sgc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:sgc>/data
)

add_custom_target(zip_sgc ALL
    COMMAND ${CMAKE_COMMAND} -E tar cfv
        ${CMAKE_CURRENT_SOURCE_DIR}/dest/SGC.zip
        --format=zip
        SGC
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dest
    DEPENDS sgc
)