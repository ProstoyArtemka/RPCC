cmake_minimum_required(VERSION 3.16)
project(rpcc_driver)

set(CMAKE_C_STANDARD 99)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

include_directories(include)
include_directories(libs)
file(GLOB SOURCES "src/*.c")

if (WIN32)
    list(APPEND SOURCES resource.rc)

endif()

add_executable(${PROJECT_NAME} ${SOURCES}) 

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(${PROJECT_NAME} raylib comctl32 onecore comdlg32)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
# if(WIN32)
#     target_compile_options(${PROJECT_NAME} PRIVATE -mwindows)
#     target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
# endif()