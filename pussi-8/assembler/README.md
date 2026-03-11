Installer.

README in progress.

Manual installation for your system:

- Make sure your have a compiling suite as well as `zlib` installed.
- `make`

Portable installation (for Linux systems, except for NixOS because yes):

- Make sure you have `musl-gcc` installed.
- Install zlib statically:  
  ```bash
  git clone git@github.com:madler/zlib.git
  cd zlib
  ./configure --static --prefix=/usr/local/musl
  sudo make install
  ```
- Go back to the `assembler` directory and run `make portable`

Both installation methods will create a `./assembli` binary file.

This installation does not work for Windows machines, please take a look at
[WSL](https://learn.microsoft.com/en-us/windows/wsl/install).

## Todo

- warning for using r0/r7 in operations that use them
- more metadata in schematic?
- coverage, add tests, add functional tests
- import files in source file
- better and faster jumps with NOP reduction
- warning for useless operations (move %r0,%r0)
- handle negative numbers
- warnings for operations (except xor r,r) after side-effects from aliases
