add_subdirectory(engine)

add_executable(amphora game_loop.c)
target_link_libraries(amphora PRIVATE amphora_engine $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>)
