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

### Manual local installation for linux systems:

- Make sure your have a compiling suite as well as `zlib` installed.
- `make`

### Portable installation for Linux systems:

- Install `musl-gcc`
- Install zlib statically:  
  ```bash
  git clone git@github.com:madler/zlib.git
  cd zlib
  ./configure --static --prefix=/usr/local/musl
  sudo make install
  ```
- Go back to the `assembler` directory and run `make portable`

### Windows (64bit) installation:

- Install `mingw-w64-gcc`, `mingw-w64-zlib`; these are the AUR package names
  because I use Arch (btw), I don't know about other distros' but they should be
  similar.
- Run `make windows`

Building multiple targets will most likely require a `make clean` in between.

All installation methods will create a `./assembli` binary file.

## Todo

- handle ctr/rtc
- warning for using r0/r7 in operations that use them
- more metadata in schematic?
- coverage, add tests, add functional tests
- import files in source file
- better and faster jumps with NOP reduction
- warning for useless operations (move %r0,%r0)
- handle negative numbers, update docs
- warnings for operations (except xor r,r) after side-effects from aliases
