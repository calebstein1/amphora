cmake_minimum_required(VERSION 3.21)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(amphora LANGUAGES C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(VCPKG_LIBRARY_LINKAGE static)

if(NOT WIN32)
	string(APPEND CMAKE_C_FLAGS_DEBUG " -fsanitize=address,undefined")
endif()
string(APPEND CMAKE_C_FLAGS_DEBUG " -Werror=all -Werror=pedantic -Wextra")

find_package(SDL2 CONFIG REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
