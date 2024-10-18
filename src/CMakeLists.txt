add_subdirectory(engine)

add_executable(amphora game_loop.c)

if(WIN32)
	target_sources(amphora PRIVATE engine/bin_loader.rc)
else()
	set(BIN_LOADER_SRC ${CMAKE_SOURCE_DIR}/src/engine/bin_loader.S)
	set(BIN_LOADER_INTER ${CMAKE_BINARY_DIR}/src/engine/bin_loader_pre.S.m4)
	set(BIN_LOADER_FINAL ${CMAKE_BINARY_DIR}/src/engine/bin_loader_final.S)
	add_custom_command(
		OUTPUT ${BIN_LOADER_INTER}
		COMMAND ${CMAKE_C_COMPILER} -I${CMAKE_SOURCE_DIR}/include -E ${BIN_LOADER_SRC} -o ${BIN_LOADER_INTER}
		DEPENDS ${BIN_LOADER_SRC}
	)
	add_custom_command(
		OUTPUT ${BIN_LOADER_FINAL}
		COMMAND m4 ${BIN_LOADER_INTER} > ${BIN_LOADER_FINAL}
		DEPENDS ${BIN_LOADER_INTER}
	)
	target_sources(amphora PRIVATE ${BIN_LOADER_FINAL})
endif()
target_link_libraries(amphora PRIVATE amphora_engine $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>)
target_link_libraries(amphora PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
