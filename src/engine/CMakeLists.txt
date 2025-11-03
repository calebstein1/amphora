add_library(amphora_engine SHARED
	main.c
	render.c
	img.c
	input.c
	events.c
	ttf.c
	tilemap.c
	db.c
	save_data.c
	mixer.c
	prefs.c
	random.c
	collision.c
	error.c
	lib.c
	scenes.c
	ht_hash.c
	session_data.c
	particles.c
	typewriter.c
	memory.c
)

target_link_libraries(amphora_engine PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
	$<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
	$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
	unofficial::sqlite3::sqlite3
)
if(LINUX)
	target_link_libraries(amphora_engine PRIVATE unofficial::UUID::uuid)
endif()

target_compile_options(amphora_engine PRIVATE -fPIC)
