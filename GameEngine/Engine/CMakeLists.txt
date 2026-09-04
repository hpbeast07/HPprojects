add_library(GameEngineLib STATIC
    # Core
    Core/Logger.cpp
    Core/Window.cpp
    Core/Application.cpp
    Core/Paths.cpp
    # Input
    Input/Input.cpp
    # Renderer
    Renderer/Shader.cpp
    Renderer/Buffer.cpp
    Renderer/Camera.cpp
    Renderer/Texture.cpp
    Renderer/Framebuffer.cpp
    Renderer/Material.cpp
    Renderer/Mesh.cpp
    Renderer/Renderer.cpp
    Renderer/OpenGL/OpenGLRenderAPI.cpp
    # Scene
    Scene/Scene.cpp
    Scene/World.cpp
)

target_include_directories(GameEngineLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/stb
)

target_link_libraries(GameEngineLib PUBLIC
    glad
    glfw
    glm::glm
    imgui
)

# GLFW must not include the platform OpenGL headers before GLAD.
target_compile_definitions(GameEngineLib PUBLIC GLFW_INCLUDE_NONE)

# Warnings
if(MSVC)
    target_compile_options(GameEngineLib PRIVATE /W4 /WX-)
    if(GE_WARNINGS_AS_ERRORS)
        target_compile_options(GameEngineLib PRIVATE /WX)
    endif()
else()
    target_compile_options(GameEngineLib PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
    if(GE_WARNINGS_AS_ERRORS)
        target_compile_options(GameEngineLib PRIVATE -Werror)
    endif()
endif()

# For MinGW threading
if(MINGW)
    target_link_libraries(GameEngineLib PUBLIC -lpthread)
endif()
