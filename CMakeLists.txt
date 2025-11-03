cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
	set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
	set(VCPKG_LIBRARY_LINKAGE static)
endif()

if (APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    # set(CMAKE_MACOSX_BUNDLE ON)
endif()

# Set project name here
project(amphora LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(WIN32)
	string(APPEND CMAKE_C_FLAGS_DEBUG " /DDEBUG /W4 /WX")
	string(APPEND CMAKE_CXX_FLAGS_DEBUG " /DDEBUG /W4 /WX")
else()
	string(APPEND CMAKE_C_FLAGS_DEBUG " -DDEBUG -fsanitize=address,undefined -Werror=all -Werror=pedantic -Wextra")
	string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DDEBUG -fsanitize=address,undefined -Wall -Wextra")
endif()

if(EMSCRIPTEN)
	string(APPEND CMAKE_C_FLAGS " -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_Mixer=2")
else()
	find_package(SDL2 CONFIG REQUIRED)
	find_package(SDL2_image CONFIG REQUIRED)
	find_package(SDL2_mixer CONFIG REQUIRED)
	find_package(SDL2_ttf CONFIG REQUIRED)
	find_package(unofficial-sqlite3 CONFIG REQUIRED)
	if(LINUX)
		find_package(unofficial-libuuid CONFIG REQUIRED)
	endif()
endif()

include_directories(
	include
	${SDL2_INCLUDE_DIRS}
)

add_subdirectory(src)
file(COPY content DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
