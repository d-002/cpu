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
