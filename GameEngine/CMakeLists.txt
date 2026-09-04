cmake_minimum_required(VERSION 3.25)
project(GameEngine VERSION 0.2.0 LANGUAGES C CXX)

# ── C++20 globally ────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GE_BUILD_EDITOR "Build the visual editor executable" ON)
option(GE_BUILD_RUNTIME "Build the standalone runtime executable" ON)
option(GE_WARNINGS_AS_ERRORS "Treat engine compiler warnings as errors" OFF)

# ── Output directories ────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── Build type ────────────────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ── Compile definitions ───────────────────────────────────────────────────────
# These generator expressions work with both single- and multi-config generators.
add_compile_definitions(
    $<$<CONFIG:Debug>:GE_BUILD_DEBUG>
    $<$<PLATFORM_ID:Windows>:GE_PLATFORM_WINDOWS>
)

# ── FetchContent dependencies ─────────────────────────────────────────────────
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)  # Cache downloads

# ── GLFW ─────────────────────────────────────────────────────────────────────
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ── GLM ───────────────────────────────────────────────────────────────────────
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# ── Dear ImGui (docking branch) ───────────────────────────────────────────────
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)
# ImGui's OpenGL backend must use the same generated OpenGL loader as the engine.
# This must be a target compile definition; setting a CMake cache variable alone
# does not affect compilation of imgui_impl_opengl3.cpp.
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD2)

# ── GLAD (vendored — OpenGL 4.6 core loader) ──────────────────────────────────
add_library(glad STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/gl.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)
if(WIN32)
    target_link_libraries(glad PUBLIC opengl32)
else()
    target_link_libraries(glad PUBLIC GL)
endif()

# ── Engine library ────────────────────────────────────────────────────────────
add_subdirectory(Engine)

# ── Editor executable ─────────────────────────────────────────────────────────
if(GE_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

# ── Runtime executable ────────────────────────────────────────────────────────
if(GE_BUILD_RUNTIME)
    add_subdirectory(Runtime)
endif()

message(STATUS "GameEngine configured successfully")
message(STATUS "  C++ standard : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler     : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
