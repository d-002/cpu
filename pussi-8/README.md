# PUSSI-8

README for the PUSSI-8 Minecraft computer.

The assembler uses libnbt by Celisium.

> [!WARNING]
> This project is still under development.

## Known limitations, could be fixed in a later CPU

- Relative jumps can only jump forward
- Not many instructions in the page cache
- Unneccesary pointer access for IO ports in OUT instruction

Assembly language / instruction set documentation:

[Documentation](https://github.com/d-002/cpu/tree/main/pussi-8/assembler/docs)
