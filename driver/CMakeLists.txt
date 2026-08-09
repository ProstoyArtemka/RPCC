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

file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES}) 

target_link_libraries(${PROJECT_NAME} raylib comctl32 onecore)

# if(WIN32)
#     target_compile_options(${PROJECT_NAME} PRIVATE -mwindows)
#     target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
# endif()