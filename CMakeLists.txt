cmake_minimum_required(VERSION 3.21)

file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(amphora LANGUAGES C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(VCPKG_LIBRARY_LINKAGE static)

if(WIN32)
	string(APPEND CMAKE_C_FLAGS_DEBUG " /W4 /WX")
else()
	string(APPEND CMAKE_C_FLAGS_DEBUG " -fsanitize=address,undefined -Werror=all -Werror=pedantic -Wextra")
endif()

find_package(SDL2 CONFIG REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
