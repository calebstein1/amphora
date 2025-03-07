# Amphora

Amphora is an advanced game engine for making 2D games.

## Usage

Amphora is distributed in source form and its engine components reside in `src/engine` and `include/engine`.
This makes it easy to tweak the internals of the engine as needed by the project.
Non-engine game files are located in `src` and `include`.

Amphora games are written in C or C++ with SDL2 and extra utility functions provided by the engine.

Configuration options can be set in `include/config.h` and `include/colors.h`.
Resource names and paths are specified in `include/resources.h`.

Note that certain functions that allocate memory, like `Amphora_CreateSprite` may cause stutters in gameplay when built using the Debug configuration.
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

#### Note for Linux builds

If you intend to use the built-in audio system and are building for Linux, you will need to make sure you have your audio library headers installed _before_ vcpkg builds SDL2_mixer.

This can be accomplished on Ubuntu/Debian-based systems with the following command:

```shell
sudo apt install libasound2-dev libpulse-dev
```

Alternatively, you can modify the build scripts to use system libraries and skip vcpkg altogether, but that's far beyond the scope of this document.
