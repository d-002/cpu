import os
import time
import enum
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


class Label:
    name: str
    index: int

    def __init__(self, name: str, index) -> None:
        self.name = name
        self.index = index

    def __repr__(self) -> str:
        return f'Label<{self.name}, {self.index}>'


class Instruction:
    opcode: OpcodeEnum
    args: list[str]

    def __init__(self, opcode: str, args: list[str]):
        self.opcode = OpcodeEnum[opcode.upper()]
        self.args = args

    def __repr__(self) -> str:
        return f'Instruction<{self.opcode.name}, {self.args}>'


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
                    raise ValueError('Invalid syntax: ' + line)
                opcode = parts[0].strip()
                args = (
                    []
                    if len(parts) == 1
                    else [arg.strip() for arg in parts[1].split(',')]
                )

                instructions.append(Instruction(opcode, args))
                instruction_index += 1

        except Exception:
            print(f'\nEncountered an error at {path}:{file_index + 1}')
            raise

    return labels, instructions


def assemble(filename: str) -> None:
    """
    Assemble a Stripe assembly file into the corresponding binary and schematic
    files.
    """
    source_file = join(INPUT_DIR, filename + INPUT_EXTENSION)
    bin_file = join(OUTPUT_DIRS[0], filename + OUTPUT_EXTENSIONS[0])
    schematic_file = join(OUTPUT_DIRS[1], filename + OUTPUT_EXTENSIONS[1])

    labels, instructions = parse_instructions(source_file)
    print(labels)
    print(instructions)


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
        if all(output.get(path, 0) < timestamp for output in outputs)
    ]

    print(f'Found {len(to_process)} files to process.')

    t0 = time.time()
    for i, file in enumerate(to_process):
        print(f'Processing "{file}" ({i + 1}/{len(to_process)})', end='\r')
        assemble(file)
    t1 = time.time()

    print(f'Done in {(t1 - t0) * 1000:.3f}ms.'.ljust(80))


if __name__ == '__main__':
    main()
