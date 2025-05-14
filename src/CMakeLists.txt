add_subdirectory(engine)

add_executable(${CMAKE_PROJECT_NAME} Scenes/Level1.cpp
	Scenes/Level2.c
	FX/motion_blur.c
	FX/snowfall.c)

if(EMSCRIPTEN)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
		amphora_engine
		${SDL2_LIBRARIES}
	)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
		amphora_engine
		$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>
		$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
		$<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
		$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
		unofficial::sqlite3::sqlite3
	)
	if(LINUX)
		target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE unofficial::UUID::uuid)
	endif()
endif()
