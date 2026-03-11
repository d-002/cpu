# Assembly language

## General

The [assembler](https://github.com/d-002/cpu/tree/main/pussi-8/assembler) parses
and exports a program into both a binary file and a schematic.

This program is written in a custom assembly language, although in practice it
is similar to
[Intel assembly](https://en.wikipedia.org/wiki/X86_assembly_language).
Files should use the `.psi` file extension.

It should be noted that PUSSI-8 can mainly handle 8-bit numbers.
The choice of considering them as signed or unsigned integers, or even
floating-point numbers, is up to the programmer.
Though, the assembler currently does not support parsing negative numbers.

PUSSI-8 does handle 16 or even 32-bit numbers in the case of the program counter
and timer respectively, though the instructions that deal with them require the
use of multiple registers for addressing, or only reading from part of the wider
number.

## Program structure and lines

A program is a list of lines that are parsed and executed sequentially.
A line must be one of:

### Blank line

This is a line composed only of whitespaces.

Example:

```psi
        ; multiple whitespaces followed by a comment. Spaces may be e.g. tabs.
```

### Macro definition

The format for a macro definision is `macro_name = macro_value`.

This allows to define more explicit names for registers, memory addresses, ports
or even other macros (recursive expansion) for code readability.

Example:

```psi
foo = %r0
```

### Instruction, with optional label

Instructions are run by the CPU.
They must be either regular instructions or aliases (see below).

An instruction has to be indented by at least one space, to distinguish it from
a macro definition.
It always includes an opcode, followed by zero or more operands.

These operands may be registers, memory addresses or ports (see below).
They may also be macros, which will be substituted by their respective values,
or labels, which will be replaced by their instruction index after aliases
expansion is complete.

Operands must be separated from the opcode with one or more spaces, and must be
separated from one another with a single comma and optional spaces.

No checks are done for whether a label is appropriate, to allow for greater
flexibility with using the call stack for example.

Additionally, one may define labels, which need to be at the very start of the
line.
These must start with a dot to indicate that they are a label and not a macro,
but when using them in instructions the dot must be omitted.

It is also possible to add a colon after a label for stylistic purposes, but
this colon may be omitted.
My personal preference is to use a colon to further distinguish labels from
macros.

Examples:

```psi
    nop
```

```psi
.loop: add %r1,%r2
```

```psi
.start
```

## Comments

This assembly language supports comments, which are optional notices to readers.
They start with `;`.
Everything after and including this semicolon is ignored.

This means that a line with only spaces and a comment is considered to be blank.

## Identifiers

Macros and labels must be made of consecutive alphanumeric characters.
They may contain underscores, but must not start with a number.

Macros and labels must not be redefined and must not share the same name as an
identifier of the other type.

## Registers, memory and ports

To address registers, one may use the syntax `%rn`, where `n` is the name /
index of the register.
For example, `%r5` refers to the 6th register (index 5, starting from 0).

Aliases (see below) also allow the use of different data types such as memory
(`%mn`, where `n` is an address) and ports (`%pn`, where `n` is the port name /
index).

## Numbers

Numbers may be written in decimal form.
They must not start with leading zeroes.
For example, `10` refers to the number 10.

They may also be written in hexadecimal form, in this case they must be prefixed
with `0x`.
For example, `0x10` refers to the number 16.

Finally, numbers may be written in binary form, in this case they must be
prefixed with `0b`.
For example, `0b10` refers to the number 2.

## Instruction set and aliases

The full instruction set and specifications can be found [here](
https://github.com/d-002/cpu/blob/main/pussi-8/assembler/docs/instruction_set.md).

The provided assembler supports quality-of-life aliases, exposed [here](
https://github.com/d-002/cpu/blob/main/pussi-8/assembler/docs/aliases.md).
