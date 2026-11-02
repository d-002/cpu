import os
import time
from os.path import splitext, join, getmtime, basename

INPUT_DIR = 'programs'
OUTPUT_DIRS = ['bin', 'schematics']
INPUT_EXTENSION = '.asm'
OUTPUT_EXTENSIONS = ['.bin', '.litematic']

os.makedirs(INPUT_DIR, exist_ok=True)
for dir in OUTPUT_DIRS:
    os.makedirs(dir, exist_ok=True)


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


def assemble(filename: str) -> None:
    """
    Assemble a Stripe assembly file into the corresponding binary and schematic
    files.
    """
    source_file = join(INPUT_DIR, filename + INPUT_EXTENSION)
    bin_file = join(OUTPUT_DIRS[0], filename + OUTPUT_EXTENSIONS[0])
    schematic_file = join(OUTPUT_DIRS[1], filename + OUTPUT_EXTENSIONS[1])


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
