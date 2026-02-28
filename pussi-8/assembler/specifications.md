# assembli

Parse and assemble `.psi` (text) files into the corresponding binary code and
schematic.

Syntax: `assembli [--verbose] [--print] [--norun] [--help] { file }`
- `--verbose`: verbose
- `--print`: prettyprint the compiled files
- `--norun`: don't assemble; useful in conjunction with `--print`
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
