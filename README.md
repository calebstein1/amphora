# Amphora

Amphora is an advanced game engine for making beautiful monochromatic 2d pixel-art games.

## Usage

Amphora is distributed in source form and its engine components reside in `src/engine` and `include/engine`.
This makes it easy to tweak the internals of the engine as needed by the project.
Non-engine game files are located in `src` and `include`.

Amphora games are written in C with SDL2 and extra utility functions provided by the engine.
There is a demo skeleton project demonstrating the structure in `src/game_loop.c` and `include/config.h`.

In the C sources, there are two required functions: `void game_init(void)`, and `void game_loop(Uint64 frame, const input_state *key_actions, struct save_data_t *save_data)`.
`game_init` is called once at game start, and `game_loop` runs once per frame.

Configuration options can be set in `include/config.h`.
These options include the framerate, window mode, resolution, key mapping, and save data structure.

You can see how some of the built-in functions work, as well as how to use certain built-in structures like `input_state` by looking at the provided example `game_loop.c` and `config.h` files.

More documentation on built-in functions, and how the sprite/zone system works will be coming soon, and there is a custom sprite editor in development.
