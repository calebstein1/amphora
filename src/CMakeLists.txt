add_subdirectory(engine)

add_executable(amphora game_loop.c)
target_link_libraries(amphora amphora_engine ${SDL2_LIBRARIES})
