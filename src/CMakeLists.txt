add_subdirectory(engine)

add_executable(${CMAKE_PROJECT_NAME} Scenes/Level1.cpp
	Scenes/Level2.c
	FX/motion_blur.c
	FX/snowfall.c
	Entities/Player.cpp
	Entities/Player.h
	Scenes/SceneList.c)

if(EMSCRIPTEN)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
		amphora_engine
		${SDL2_LIBRARIES}
	)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE amphora_engine)
endif()
