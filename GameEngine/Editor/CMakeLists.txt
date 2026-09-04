add_executable(GameEngineEditor
    main.cpp
)

target_include_directories(GameEngineEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(GameEngineEditor PRIVATE
    GameEngineLib
)

# On Windows, hide the console window for Release builds
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(GameEngineEditor PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Copy Assets to binary directory when a project supplies them.
if(EXISTS "${CMAKE_SOURCE_DIR}/Assets")
    add_custom_command(TARGET GameEngineEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Assets
            $<TARGET_FILE_DIR:GameEngineEditor>/Assets
        COMMENT "Copying Assets to output directory"
    )
endif()

set_target_properties(GameEngineEditor PROPERTIES
    OUTPUT_NAME "GameEngineEditor"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
