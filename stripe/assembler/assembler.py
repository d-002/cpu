import os
import time
import sys
import re
import getpass
import argparse
from os.path import splitext, join, getmtime
from litemapy import Schematic, Region, BlockState  # pyright: ignore

from common import OpcodeEnum

INPUT_DIR = 'programs'
OUTPUT_DIRS = ['bin', 'schematics']
INPUT_EXTENSION = '.asm'
OUTPUT_EXTENSIONS = ['.bin', '.litematic']

parser = argparse.ArgumentParser(
    prog='Stripe assembler',
    description='Assembler and code checker for Stripe .str files',
)
parser.add_argument(
    '-f', '--force', action='store_true', help='force re-assemble'
)
parser.add_argument(
    '-v',
    '--verbose',
    action='store_true',
    help='display extra information when assembling',
)
args = parser.parse_args()


args_len = {
    OpcodeEnum.NOP: 0,
    OpcodeEnum.LDI: 1,
    OpcodeEnum.MOV: 2,
    OpcodeEnum.SUB: 2,
    OpcodeEnum.XOR: 2,
    OpcodeEnum.AND: 2,
    OpcodeEnum.OR: 2,
    OpcodeEnum.ADD: 2,
    OpcodeEnum.JMP: 1,
    OpcodeEnum.JZ: 1,
    OpcodeEnum.JC: 1,
    OpcodeEnum.PIX: 2,
    OpcodeEnum.RST: 0,
    OpcodeEnum.IN: 1,
    OpcodeEnum.HLT: 0,
}


class Label:
    name: str
    value: int

    def __init__(self, name: str, index: int) -> None:
        self.name = name
        self.value = index

    def __repr__(self) -> str:
        return f'Label<{self.name}, {self.value}>'


class Instruction:
    line_text: str
    line_index: int
    opcode: OpcodeEnum
    args: list[str]
    parsed_args: list[int] | None

    def __init__(
        self, line_text: str, line_index: int, opcode: str, args: list[str]
    ):
        opcode = opcode.upper()

        if opcode not in OpcodeEnum:
            raise ValueError(f'Opcode not found: "{opcode}"')

        self.line_text = line_text
        self.line_index = line_index
        self.opcode = OpcodeEnum[opcode]
        self.args = args
        self.parsed_args = None

        if len(args) != args_len[self.opcode]:
            raise ValueError(
                f'Invalid number of arguments for {opcode}: '
                f'expected {args_len[self.opcode]} but got {len(args)}'
            )

    def __repr__(self) -> str:
        args = self.parsed_args or self.args
        return f'Instruction<{self.opcode.name}, {args}>'

    def resolve(self, labels: dict[str, int]):
        parsed_args: list[int] = []

        for arg in self.args:
            if re.match('^r[0-7]$', arg):
                value = int(arg[1])
            elif re.match('^[0-9]+$', arg):
                value = int(arg)
                modulo = (
                    64
                    if self.opcode
                    in (
                        OpcodeEnum.LDI,
                        OpcodeEnum.JMP,
                        OpcodeEnum.JZ,
                        OpcodeEnum.JC,
                    )
                    else 8
                )
                if not 0 <= value < modulo:
                    print(
                        '\033[33mWarning: Found value outside the [0, '
                        f'{modulo}) range for {self.opcode}: {value}\033[m',
                        file=sys.stderr,
                    )
                value %= modulo
            else:
                value = labels.get(arg)
                if value is None:
                    raise ValueError(f'Could not find label with name "{arg}"')

            parsed_args.append(value)

        self.parsed_args = parsed_args

    def encoded(self) -> tuple[int, str]:
        """
        Return both the encoded (as an integer) machine code, and the comment to
        add directly after in the bin file.
        """

        if self.parsed_args is None:
            raise ValueError(f'Instruction not resolved: {self}')

        opcode_value = list(OpcodeEnum).index(self.opcode)
        match self.opcode:
            case OpcodeEnum.NOP | OpcodeEnum.RST | OpcodeEnum.HLT:
                arg_value = 0
            case (
                OpcodeEnum.LDI
                | OpcodeEnum.JMP
                | OpcodeEnum.JZ
                | OpcodeEnum.JC
                | OpcodeEnum.IN
            ):
                arg_value = self.parsed_args[0]
            case (
                OpcodeEnum.MOV
                | OpcodeEnum.SUB
                | OpcodeEnum.XOR
                | OpcodeEnum.AND
                | OpcodeEnum.OR
                | OpcodeEnum.ADD
                | OpcodeEnum.PIX
            ):
                arg_value = (self.parsed_args[1] << 3) + self.parsed_args[0]

        comment = f' # {self.line_index}:{self.line_text}'
        return (opcode_value << 6) + arg_value, comment


def list_files(parent_dir: str, extension: str) -> dict[str, float]:
    """
    List files in a directory with the given extension, returning a dict of
    {relative filename without extension: modification timestamp}
    """

    files: dict[str, float] = {}
    for file in os.listdir(parent_dir):
        if splitext(file)[1] != extension:
            continue

        path = join(parent_dir, file)
        files[splitext(file)[0]] = getmtime(path)

    return files


def parse_instructions(path: str) -> tuple[list[Label], list[Instruction]]:
    labels: list[Label] = []
    instructions: list[Instruction] = []

    with open(path) as f:
        lines = f.read().split('\n')

    instruction_index = 0
    for file_index, line in enumerate(lines):
        line = line.split('#')[0].strip()
        if not line:
            continue

        try:
            if line[0] == '.':
                labels.append(Label(line, instruction_index))
            else:
                parts = [
                    part.strip()
                    for part in re.split(' |,', line)
                    if part.strip()
                ]
                opcode = parts[0]
                args = parts[1:]

                instructions.append(Instruction(line, file_index, opcode, args))
                instruction_index += 1

        except Exception as e:
            print(
                f'\033[31mError:\033[33m at {path}:{file_index + 1}:',
                file=sys.stderr,
            )
            print(f'\033[31m{e}\033[m', file=sys.stderr)
            exit(1)

    if len(instructions) > 64:
        raise ValueError(
            f'Too many instructions: uses {len(instructions)} out of 64'
        )

    return labels, instructions


def resolve_labels(
    labels: list[Label], instructions: list[Instruction]
) -> None:
    """
    Resolve all labels inside instructions from their values.
    Edit the list of instructions in-place.
    """

    labels_dict: dict[str, int] = {}
    for label in labels:
        if label.name in labels_dict:
            raise ValueError(f'Found duplicate label name: "{label.name}"')
        labels_dict[label.name] = label.value

    for instruction in instructions:
        instruction.resolve(labels_dict)


def encode_instructions(
    instructions: list[Instruction],
) -> list[tuple[int, str]]:
    """
    Encode all instructions in the list into a list of numbers, which are the
    actual MSB machine instructions.

    Return a list of tuples:

    (
        machine code,
        comment to add right after in the bine file
    )
    """

    encoded: list[tuple[int, str]] = []

    for instruction in instructions:
        encoded.append(instruction.encoded())

    return encoded


def assemble(filename: str) -> None:
    """
    Assemble a Stripe assembly file into the corresponding binary and schematic
    files.
    """

    source_file = join(INPUT_DIR, filename + INPUT_EXTENSION)
    bin_file = join(OUTPUT_DIRS[0], filename + OUTPUT_EXTENSIONS[0])
    schematic_file = join(OUTPUT_DIRS[1], filename + OUTPUT_EXTENSIONS[1])

    # assembling
    if args.verbose:
        print('-- Parsing instructions')
    labels, instructions = parse_instructions(source_file)

    if args.verbose:
        print(
            f'-- Found {len(instructions)}/64 instructions,',
            f'or {round(len(instructions) / 64 * 100)}% of ROM storage.',
        )
        print('-- Resolving labels')
    resolve_labels(labels, instructions)

    if args.verbose:
        print('-- Encoding instructions')
    encoded = encode_instructions(instructions)

    # bin export
    if args.verbose:
        print(f'-- Exporting to {bin_file}')
    with open(bin_file, 'w') as f:
        for instruction in encoded:
            f.write(f'{bin(instruction[0])[2:]:0>10}{instruction[1]}\n')

    # schematic export
    if args.verbose:
        print(f'-- Exporting to {schematic_file}')
    reg = Region(0, 0, 0, 15, 15, 20)
    schem = Schematic(
        name=filename,
        author=getpass.getuser(),
        regions={filename: reg},
        lm_version=5,
    )

    index = 0
    for y in range(8):
        for x in range(8):
            value = 0 if index >= len(encoded) else encoded[index][0]
            index += 1

            for z in range(10):
                bit = value & 1
                value >>= 1
                if bit == 0:
                    block = BlockState('minecraft:red_wool')
                else:
                    if (x + y) % 2:
                        kwargs = {'east': 'up'}
                    else:
                        kwargs = {'west': 'up'}
                    block = BlockState(
                        'minecraft:redstone_wire',
                        north='side',
                        south='side',
                        **kwargs,
                    )

                reg[14 - x * 2, y * 2, z * 2 + (z > 2)] = block

    schem.save(schematic_file)


def main() -> None:
    os.makedirs(INPUT_DIR, exist_ok=True)
    for dir in OUTPUT_DIRS:
        os.makedirs(dir, exist_ok=True)

    programs = list_files(INPUT_DIR, INPUT_EXTENSION)
    outputs = [
        list_files(dir, extension)
        for dir, extension in zip(OUTPUT_DIRS, OUTPUT_EXTENSIONS)
    ]

    to_process = [
        filename
        for filename, timestamp in programs.items()
        if args.force
        or any(output.get(filename, 0) < timestamp for output in outputs)
    ]

    print(f'Found {len(to_process)} files to process.')

    t0 = time.time()
    for i, file in enumerate(to_process):
        percentage = round((i + 1) / len(to_process) * 100)
        print(f'[{percentage:3}%] \033[32mBuilding STR {file}\033[m')
        assemble(file)
        print(f'[{percentage:3}%] \033[1;32mBuilt target {file}\033[m')
    t1 = time.time()

    s = 's' if len(to_process) > 1 else ''
    print(f'Built {len(to_process)} target{s} in {(t1 - t0) * 1000:.3f}ms.')


if __name__ == '__main__':
    main()
