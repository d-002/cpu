# assembli

Parse and assemble `.psi` (text) files into the corresponding binary code and
schematic.

Syntax: `assembli [--verbose] [--print] [--help] { --file file }`
- `--verbose`: verbose
- `--print`: only print, parse and run checks, don't assemble
- `--file`: assemble file, can specify multiple files to assemble in this order
- `--help`: print an error message to stdout

## Todo

- help message
- assemble to file
- ctr / rtc aliases
- handle jumps that actually work differently (expand aliases to whichever works
  best)
- assemble to schematic
- display multiple errors if possible
- handle negative numbers
- todo in code
