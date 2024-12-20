add_subdirectory(engine)

add_executable(amphora game_loop.cpp)

if(WIN32)
	target_sources(amphora PRIVATE engine/bin_loader.rc)
else()
	set(BIN_LOADER_SRC ${CMAKE_SOURCE_DIR}/src/engine/bin_loader.S.m4)
	set(BIN_LOADER_INTER ${CMAKE_BINARY_DIR}/src/engine/bin_loader.prepro.S.m4)
	set(BIN_LOADER_FINAL ${CMAKE_BINARY_DIR}/src/engine/bin_loader.S)
	add_custom_command(
		OUTPUT ${BIN_LOADER_INTER}
		COMMAND ${CMAKE_C_COMPILER} -xc -I${CMAKE_SOURCE_DIR}/include -E ${BIN_LOADER_SRC} -o ${BIN_LOADER_INTER}
		DEPENDS ${BIN_LOADER_SRC}
	)
	add_custom_command(
		OUTPUT ${BIN_LOADER_FINAL}
		COMMAND m4 ${BIN_LOADER_INTER} > ${BIN_LOADER_FINAL}
		DEPENDS ${BIN_LOADER_INTER}
	)
	target_sources(amphora PRIVATE ${BIN_LOADER_FINAL})
endif()
if(EMSCRIPTEN)
	target_link_libraries(amphora PRIVATE
		amphora_engine
		${SDL2_LIBRARIES}
	)
else()
	target_link_libraries(amphora PRIVATE
		amphora_engine
		$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>
		$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
		$<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
		$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
		unofficial::sqlite3::sqlite3
	)
	if(LINUX)
		target_link_libraries(amphora PRIVATE unofficial::UUID::uuid)
	endif()
endif()
