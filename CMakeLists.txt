cmake_minimum_required(VERSION 3.10)
project(systemSolaire)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")

option(USE_SANITIZER "Enable AddressSanitizer" OFF)
if(USE_SANITIZER)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(include external /usr/include)

include_directories(external/imgui)
include_directories(external/imgui/backends)

add_library(glad STATIC external/glad/glad.c)
target_include_directories(glad PUBLIC external external/glad)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC 
    external/imgui
    external/imgui/backends
)

add_executable(systemSolaire
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
    src/camera_ubo.cpp
    src/transform_ubo.cpp
    src/model.cpp
    src/imgui_impl.cpp  
    external/tiny_obj_loader.cc
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(systemSolaire PRIVATE 
    glad 
    imgui
    glfw 
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

target_compile_options(systemSolaire PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

target_compile_options(imgui PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "ImGui support: ENABLED")
