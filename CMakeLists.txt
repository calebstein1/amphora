cmake_minimum_required(VERSION 3.21)

project(amphora LANGUAGES C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

string(APPEND CMAKE_C_FLAGS_DEBUG " -fsanitize=address,undefined -Werror=all -Werror=pedantic -Wextra")

find_package(SDL2 REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
