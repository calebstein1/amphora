add_subdirectory(engine)

add_executable(amphora game_loop.c)
target_link_libraries(amphora amphora_engine ${SDL2_LIBRARIES})

add_custom_command(
        TARGET amphora POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/content $<TARGET_FILE_DIR:amphora>/content)
