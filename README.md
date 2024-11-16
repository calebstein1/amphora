# Amphora

Amphora is an advanced game engine for making 2D games.

## Usage

Amphora is distributed in source form and its engine components reside in `src/engine` and `include/engine`.
This makes it easy to tweak the internals of the engine as needed by the project.
Non-engine game files are located in `src` and `include`.

Amphora games are written in C or C++ with SDL2 and extra utility functions provided by the engine.
There is a demo skeleton project demonstrating the structure in `src/game_loop.c` and `include/config.h`.

In the C sources, there are two required functions: `void game_init(void)`, and `void game_loop(Uint64 frame, const struct input_state_t *key_actions)`.
`game_init` is called once at game start, and `game_loop` runs once per frame.

Configuration options can be set in `include/config.h` and `include/colors.h`.
Resource names and paths are specified in `include/resources.h`.

You can see how some of the built-in functions work, as well as how to use certain built-in structures like `input_state` by looking at the provided example `game_loop.c` and `config.h` files.

More proper documentation will be coming soon.

Note that certain functions that allocate memory, like `create_sprite` may cause stutters in gameplay when built using the Debug configuration.
These stutters should not exist in the Release configuration.

## Building

Amphora uses CMake and vcpkg to build and manage dependencies.
You'll need to ensure that vcpkg is installed somewhere on your system and that the `VCPKG_ROOT` environment variable is set properly.
Once that's done, all you'll need to do is add any new files to `add_executable` in `src/CmakeLists.txt`, then you're ready to build your game!

### Windows

Windows builds are supported using the MSVC compiler.
MinGW and other compilers are not supported at this time.

Visual Studio (not VSCode!) works well with Amphora projects, as does CLion.
It may be possible to configure VSCode to work with Amphora, but this has not been tested.

### *nix (MacOS, Linux)

On *nix systems, you can build from the command line using the following commands.
GCC and Clang are both supported, as are x86-64 and arm64 architectures.

```
mkdir -p build && cd build
cmake ..
make
```

Any IDE with CMake support should work as well, though CLion is the only IDE that's been officially tested on MacOS and Linux.

