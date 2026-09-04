add_executable(GameEngineRuntime
    main.cpp
)

target_include_directories(GameEngineRuntime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(GameEngineRuntime PRIVATE
    GameEngineLib
)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(GameEngineRuntime PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set_target_properties(GameEngineRuntime PROPERTIES
    OUTPUT_NAME "GameEngineRuntime"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
