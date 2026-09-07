cmake_minimum_required(VERSION 4.3)

project(ImageProcessor C)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.14
        GIT_SHALLOW TRUE
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

add_executable(ImageProcessor
        bmp.h
        bmp.c
        imageprocessor.c

)

target_link_libraries(ImageProcessor PRIVATE SDL3::SDL3)