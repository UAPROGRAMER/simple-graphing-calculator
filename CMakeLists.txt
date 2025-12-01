cmake_minimum_required(VERSION 3.20)

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

    install(FILES data/icons/sgc128.png
        DESTINATION share/icons/hicolor/128x128/apps)
    
    install(FILES sgc.desktop DESTINATION share/applications)
endif ()

target_include_directories(sgc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/mini
)

if (WIN32)
    target_link_libraries(sgc glfw3 opengl32)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(sgc glfw GL)
endif ()

add_custom_command(TARGET sgc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:sgc>/data
)