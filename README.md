# Amphora

Amphora is an advanced game engine for making beautiful monochromatic 2d pixel-art games.

## Usage

Amphora is distributed in source form and its engine components reside in `src/engine` and `include/engine`.
This makes it easy to tweak the internals of the engine as needed by the project.
Non-engine game files are located in `src` and `include`.

Amphora games are written in C with SDL2 and extra utility functions provided by the engine.
There is a demo skeleton project demonstrating the structure in `src/game_loop.c` and `include/config.h`.

In the C sources, there are two required functions: `void game_init(void)`, and `void game_loop(Uint64 frame, const struct input_state_t *key_actions, struct save_data_t *save_data)`.
`game_init` is called once at game start, and `game_loop` runs once per frame.

Configuration options can be set in `include/config.h`.
These options include the framerate, window mode, resolution, key mapping, and save data structure.

You can see how some of the built-in functions work, as well as how to use certain built-in structures like `input_state` by looking at the provided example `game_loop.c` and `config.h` files.

More documentation on built-in functions, and how the sprite/zone system works will be coming soon, and there is a custom sprite editor in development.

## Building

Amphora uses CMake to build, all you need to do is add any new files to `add_executable` in `src/CmakeLists.txt`, then run the following commands in the project root:

```
mkdir -p build && cd build
cmake ..
make
```

This will place the binary in `build/bin` and should work fine on both MacOS and Linux, x86 and arm.
Windows build support in in development and will be coming soon.

