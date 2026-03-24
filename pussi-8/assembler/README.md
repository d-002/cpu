# Assembler

Assembler for the PUSSI-8.

Exports a program in a custom assembly language into both a binary file for the
[emulator](https://github.com/d-002/cpu/tree/main/pussi-8/emulator) and a
`.schem` Worldedit schematic to paste into the Minecraft computer instruction
memory.

## Documentation

[Documentation](https://github.com/d-002/cpu/tree/main/pussi-8/assembler/docs)

## Pre-built binaries

Check out the [releases](https://github.com/d-002/cpu/releases) tab.

## Building from source

### Manual local installation for Linux systems:

- Make sure your have a compiling suite as well as `zlib` installed.
- Run `make` or `make all`

### Portable installation for Linux systems:

- Install `musl-gcc`
- Install zlib statically (commands might need to be tweaked for NixOS):

  ```bash
  git clone git@github.com:madler/zlib.git
  cd zlib
  ./configure --static --prefix=/usr/local/musl
  sudo make install
  ```
- Go back to the `assembler` directory and run `make portable`

### Windows (64bit) installation:

- Install `mingw-w64-gcc`, `mingw-w64-zlib`.
  These are the AUR package names because I use Arch (btw), I don't know about
  other distros' but they should be similar.
- Run `make windows`

Building multiple targets will most likely require a `make clean` in between.

Linux installation methods will create a `./assembli` binary file, while the
Windows one will create `.\assembli.exe`.

## Todo

- handle negative numbers, update docs
- issues
- warnings for operations (except xor r,r) after side-effects from aliases
- better and faster jumps with NOP reduction
- import files in source files, multiple files ecosystem, update docs
