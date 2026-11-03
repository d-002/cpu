import re
import sys
import pygame
from abc import ABC, abstractmethod
from os.path import isfile

from common import OpcodeEnum

FPS = 60
BGCOL = (21, 24, 25)
FGCOL = (217, 220, 221)
BORDERCOL = (63, 63, 63)
LIGHT_COLS = ((41, 32, 26), (237, 199, 145))
MARGIN = 15
PIXSIZE = 30
FONTSIZE = 16


class CPU:
    instructions: list[tuple[OpcodeEnum, int, int]]
    pc: int
    instruction_cache: tuple[int, int, int]
    registers: list[int]
    values_cache: tuple[int, int]
    screen: list[list[int]]
    port: int
    flag_Z: int
    flag_C: int

    def __init__(self, instructions: list[tuple[OpcodeEnum, int, int]]) -> None:
        self.instructions = instructions
        self.reset()

    def reset(self) -> None:
        self.pc = 0
        self.instruction_cache = (0, 0, 0)
        self.registers = [0] * 8
        self.values_cache = (0, 0)
        self.screen = [[0] * 8 for _ in range(8)]
        self.port = 0
        self.flag_Z = 1
        self.flag_C = 0

        # TODO remove
        for i in range(8):
            self.screen[i][i] = 1
            self.registers[i] = i

    def prettyprint_instruction(self) -> str:
        opcode, a, b = self.instructions[self.pc]
        match opcode:
            case OpcodeEnum.NOP | OpcodeEnum.RST | OpcodeEnum.HLT:
                args = ()
            case (
                OpcodeEnum.LDI
                | OpcodeEnum.JMP
                | OpcodeEnum.JZ
                | OpcodeEnum.JC
                | OpcodeEnum.IN
            ):
                args = (str(a),)
            case (
                OpcodeEnum.MOV
                | OpcodeEnum.SUB
                | OpcodeEnum.XOR
                | OpcodeEnum.AND
                | OpcodeEnum.OR
                | OpcodeEnum.ADD
                | OpcodeEnum.PIX
            ):
                args = (str(a), str(b))

        return f'{opcode.value} {",".join(args)}'


class Sprite(ABC):
    @abstractmethod
    def should_render(self, events: list[pygame.event.Event]) -> bool:
        """
        Return whether the sprite should be rendered this frame, for example if
        the given events mean its state should change.
        """

    @abstractmethod
    def render(self) -> None:
        """
        Render the sprite on the screen.
        """


class CPUSprite(Sprite):
    cpu: CPU
    force_render: bool

    def __init__(self, cpu: CPU) -> None:
        self.cpu = cpu
        self.force_render = True

    def should_render(self, events: list[pygame.event.Event]) -> bool:
        if self.force_render:
            self.force_render = False
            return True

        return False

    def _display_bin_value(self, name: str, value: int, x: int, y: int) -> None:
        """
        Cleanly render an 8-bit value as (top to bottom): a pixel array, its
        name as text, then unsigned and signed representations.
        """

        screen.blit(
            font.render(name, True, FGCOL),
            (x, y),
        )
        screen.blit(
            font.render(str(value), True, FGCOL),
            (x, y + FONTSIZE),
        )
        screen.blit(
            font.render(str((value + 128) % 256 - 128), True, FGCOL),
            (x, y + FONTSIZE * 2),
        )

        for y in range(8):
            bit = value & 1
            value >>= 1
            pygame.draw.rect(
                screen,
                LIGHT_COLS[bit],
                (x, MARGIN + (7 - y) * PIXSIZE, PIXSIZE, PIXSIZE - 1),
            )

    def render(self) -> None:
        # screen
        pygame.draw.rect(
            screen,
            BORDERCOL,
            (MARGIN - 2, MARGIN - 2, PIXSIZE * 8 + 4, PIXSIZE * 8 + 4),
        )
        for y, line in enumerate(self.cpu.screen):
            for x, bit in enumerate(line):
                pygame.draw.rect(
                    screen,
                    LIGHT_COLS[bit],
                    (
                        MARGIN + PIXSIZE * x,
                        MARGIN + PIXSIZE * y,
                        PIXSIZE - 1,
                        PIXSIZE - 1,
                    ),
                )
        screen.blit(
            font.render('Screen', True, FGCOL),
            (MARGIN, MARGIN * 2 + PIXSIZE * 8),
        )

        # registers
        x = MARGIN * 2 + PIXSIZE * 8
        for i in range(8):
            pygame.draw.rect(
                screen,
                BORDERCOL,
                (x - 2, MARGIN - 2, PIXSIZE + 4, PIXSIZE * 8 + 4),
            )

            self._display_bin_value(
                f'r{i}', self.cpu.registers[i], x, MARGIN * 2 + PIXSIZE * 8
            )
            x += PIXSIZE + MARGIN

        # input port
        self._display_bin_value(
            f'port', self.cpu.port, x, MARGIN * 2 + PIXSIZE * 8
        )

        # PC and other info
        a, b = self.cpu.values_cache
        texts = [
            f'PC: {self.cpu.pc}/63',
            f'Flags: Z = {self.cpu.flag_Z}, C = {self.cpu.flag_C}',
            f'Values cache: A = {a}, B = {b}',
            f'Instruction to run: {self.cpu.prettyprint_instruction()}',
        ]
        y = MARGIN * 3 + PIXSIZE * 8 + FONTSIZE
        for text in texts:
            screen.blit(font.render(text, True, FGCOL), (MARGIN, y))
            y += FONTSIZE


def main(sprites: list[Sprite]) -> None:
    global running
    running = True

    while running:
        events = pygame.event.get()

        for event in events:
            if event.type == pygame.K_ESCAPE:
                running = False
            if event.type == pygame.KEYDOWN:
                running = (
                    not event.key == pygame.K_ESCAPE
                    and not event.key == pygame.K_q
                )

        should_render: set[Sprite] = set()
        for sprite in sprites:
            if sprite.should_render(events):
                should_render.add(sprite)

        if should_render:
            screen.fill(BGCOL)
            for sprite in should_render:
                sprite.render()

        pygame.display.flip()
        clock.tick(FPS)


if len(sys.argv) != 2:
    print(f'Invalid number of arguments: Expected 1, got {len(sys.argv) - 1}')
    exit(1)

bin_file = sys.argv[1]
if not isfile(bin_file) or not bin_file.endswith('bin'):
    print(f'File "{bin_file}" is not a valid Stripe .bin file')
    exit(1)

instructions: list[tuple[OpcodeEnum, int, int]] = []
with open(bin_file) as f:
    lines = f.read().split('\n')
for i, line in enumerate(lines):
    line = line.split('#')[0].strip()
    if not line:
        continue

    if not re.match('^[01]{10}$', line):
        raise ValueError(f'Failed to parse input file at line {i + 1}')
    instructions.append(
        (
            list(OpcodeEnum)[int(line[0:4], base=2)],
            int(line[4:7], base=2),
            int(line[7:10], base=2),
        )
    )

pygame.init()
pygame.display.set_caption('Sprite simulator')
clock = pygame.time.Clock()
ticks = pygame.time.get_ticks()
screen = pygame.display.set_mode((MARGIN * 11 + PIXSIZE * 17, 480))
font = pygame.font.SysFont('monospace', FONTSIZE, bold=True)

cpu = CPU(instructions)
cpu_sprite = CPUSprite(cpu)

if __name__ == '__main__':
    main([cpu_sprite])
