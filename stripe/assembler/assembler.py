import os
import time
import enum
import sys
import re
from os.path import splitext, join, getmtime, basename

INPUT_DIR = 'programs'
OUTPUT_DIRS = ['bin', 'schematics']
INPUT_EXTENSION = '.asm'
OUTPUT_EXTENSIONS = ['.bin', '.litematic']


class OpcodeEnum(str, enum.Enum):
    NOP = 'NOP'
    LDI = 'LDI'
    MOV = 'MOV'
    SUB = 'SUB'
    XOR = 'XOR'
    AND = 'AND'
    OR = 'OR'
    ADD = 'ADD'
    JMP = 'JMP'
    JZ = 'JZ'
    JC = 'JC'
    PIX = 'PIX'
    RST = 'RST'
    IN = 'IN'
    HLT = 'HLT'


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

    def __init__(self, name: str, index) -> None:
        self.name = name
        self.value = index

    def __repr__(self) -> str:
        return f'Label<{self.name}, {self.value}>'


class Instruction:
    opcode: OpcodeEnum
    args: list[str]
    parsed_args: list[int] | None

    def __init__(self, opcode: str, args: list[str]):
        opcode = opcode.upper()

        if opcode not in OpcodeEnum:
            raise ValueError(f'Opcode not found: "{opcode}"')

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
        parsed_args = []

        for arg in self.args:
            if re.match('^r[0-7]$', arg):
                value = int(arg[1])
            elif re.match('^[0-9]+$', arg):
                value = int(arg)
                if not 0 <= value <= 63:
                    print(
                        '\033[31mWarning: Found value outside',
                        f'the [0, 63] range: {value}\033[m',
                        file=sys.stderr,
                    )
                value %= 64
            else:
                value = labels.get(arg)
                if value is None:
                    raise ValueError(f'Could not find label with name "{arg}"')

            parsed_args.append(value)

        self.parsed_args = parsed_args

    def encoded(self) -> int:
        if self.parsed_args is None:
            raise ValueError(f'Instruction not resolved: {self}')

        opcode_value = 0
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
                arg_value = self.parsed_args[0] << 3
            case (
                OpcodeEnum.MOV
                | OpcodeEnum.SUB
                | OpcodeEnum.XOR
                | OpcodeEnum.AND
                | OpcodeEnum.OR
                | OpcodeEnum.ADD
                | OpcodeEnum.PIX
            ):
                arg_value = (self.parsed_args[0] << 3) + self.parsed_args[1]
            case _:
                raise ValueError(f'Case unaccounted for "{self.opcode}"')

        return (opcode_value << 6) + arg_value


def list_files(parent_dir: str, extension: str) -> dict[str, float]:
    """
    List files in a directory with the given extension, returning a dict of
    {full file path: modification timestamp}
    """

    files = {}
    for file in os.listdir(parent_dir):
        if splitext(file)[1] != extension:
            continue

        path = join(parent_dir, file)
        files[path] = getmtime(path)

    return files


def parse_instructions(path: str) -> tuple[list[Label], list[Instruction]]:
    labels = []
    instructions = []

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
                parts = line.split(' ')
                if not 1 <= len(parts) <= 2:
                    raise ValueError(f'Invalid syntax: "{line}"')
                opcode = parts[0].strip()
                args = (
                    []
                    if len(parts) == 1
                    else [arg.strip() for arg in parts[1].split(',')]
                )

                instructions.append(Instruction(opcode, args))
                instruction_index += 1

        except Exception as e:
            print(
                f'\033[31mError:\033[33m at {path}:{file_index + 1}:',
                file=sys.stderr,
            )
            print(f'\033[31m{e}\033[m', file=sys.stderr)
            exit(1)

    return labels, instructions


def resolve_labels(
    labels: list[Label], instructions: list[Instruction]
) -> None:
    """
    Resolve all labels inside instructions from their values.
    Edit the list of instructions in-place.
    """

    labels_dict = {}
    for label in labels:
        if label.name in labels_dict:
            raise ValueError(f'Found duplicate label name: "{label.name}"')
        labels_dict[label.name] = label.value

    for instruction in instructions:
        instruction.resolve(labels_dict)


def encode_instructions(instructions: list[Instruction]) -> list[int]:
    """
    Encode all instructions in the list into a list of numbers, which are the
    actual MSB machine instructions.
    """

    encoded = []

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

    labels, instructions = parse_instructions(source_file)
    resolve_labels(labels, instructions)
    encoded = encode_instructions(instructions)


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
        splitext(basename(path))[0]
        for path, timestamp in programs.items()
        if any(output.get(path, 0) < timestamp for output in outputs)
    ]

    print(f'Found {len(to_process)} files to process.')

    t0 = time.time()
    for i, file in enumerate(to_process):
        percentage = round((i + 1) / len(to_process) * 100)
        print(f'[{percentage:3}%] \033[32mBuilding STR {file}\033[m')
        assemble(file)
        print(f'[{percentage:3}%] \033[1;32mBuilt target {file}\033[m')
    t1 = time.time()

    print(f'Built {len(to_process)} targets in {(t1 - t0) * 1000:.3f}ms.')


if __name__ == '__main__':
    main()
