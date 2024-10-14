add_subdirectory(engine)

add_executable(amphora game_loop.c)

if(WIN32)
	target_sources(amphora PRIVATE engine/bin_loader.rc)
else()
	target_sources(amphora PRIVATE engine/bin_loader.S)
endif()
target_link_libraries(amphora PRIVATE amphora_engine $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>)
target_link_libraries(amphora PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
