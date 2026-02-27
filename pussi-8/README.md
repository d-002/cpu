# PUSSI-8

README for the PUSSI-8 Minecraft computer.

> [!WARNING]
> This project is still under development.

## Known limitations, could be fixed in a later CPU

- Relative jumps can only jump forward
- Not many instructions in the page cache
- Time is wasted in JUMP-like instructions, as we do not know in advance whether
  they can be expanded into 1, 2 or 3 instructions
