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
RUNDELAY = 0.3  # in seconds


class CPU:
    instructions: list[tuple[OpcodeEnum, int, int]]
    pc: int
    # instruction_cache: tuple[int, int, int]
    registers: list[int]
    # values_cache: tuple[int, int]
    screen: list[list[int]]
    port: int
    flag_Z: int
    flag_C: int

    def __init__(self, instructions: list[tuple[OpcodeEnum, int, int]]) -> None:
        self.instructions = instructions
        self.reset()

    def reset(self) -> None:
        self.pc = 0
        self.registers = [0] * 8
        self.screen = [[0] * 8 for _ in range(8)]
        self.port = 0
        self.flag_Z = 1
        self.flag_C = 0

    def get_instruction(self) -> tuple[OpcodeEnum, int, int]:
        if self.pc < len(self.instructions):
            return self.instructions[self.pc]
        return (OpcodeEnum.NOP, 0, 0)

    def _alu_op(self, res: int) -> None:
        """
        Helper to set the flags and r0 value after an ALU operation
        """

        self.registers[0] = res % 256
        self.flag_Z = int(res % 256 == 0)
        self.flag_C = int(res != res % 256)

    def prettyprint_instruction(self) -> str:
        opcode, b, a = self.get_instruction()
        match opcode:
            case OpcodeEnum.NOP | OpcodeEnum.RST | OpcodeEnum.HLT:
                args = ()
            case OpcodeEnum.LDI:
                args = (str(b * 8 + a),)
            case OpcodeEnum.JMP | OpcodeEnum.JZ | OpcodeEnum.JC | OpcodeEnum.IN:
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

    def step(self) -> None:
        opcode, b, a = self.get_instruction()
        ra, rb = self.instructions_cache = (
            self.registers[a],
            self.registers[b],
        )

        self.pc = (self.pc + 1) % 64

        match opcode:
            case OpcodeEnum.NOP:
                pass
            case OpcodeEnum.LDI:
                self.registers[0] = b * 8 + a
            case OpcodeEnum.MOV:
                self.registers[b] = ra
            case OpcodeEnum.SUB:
                self._alu_op(ra - rb)
            case OpcodeEnum.XOR:
                self._alu_op(ra ^ rb)
            case OpcodeEnum.AND:
                self._alu_op(ra & rb)
            case OpcodeEnum.OR:
                self._alu_op(ra | rb)
            case OpcodeEnum.ADD:
                self._alu_op(ra + rb)
            case OpcodeEnum.JMP:
                self.pc = ra
            case OpcodeEnum.JZ:
                if self.flag_Z:
                    self.pc = ra
            case OpcodeEnum.JC:
                if self.flag_C:
                    self.pc = ra
            case OpcodeEnum.PIX:
                self.screen[rb][ra] = 1
            case OpcodeEnum.RST:
                self.screen = [[0] * 8 for _ in range(8)]
            case OpcodeEnum.IN:
                self.registers[0] = self.port
            case OpcodeEnum.HLT:
                pass  # will be handled in the sprite that handles the loop


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


class ScreenSprite(Sprite):
    """
    Sprite for the screen section of the cpu.
    """

    cpu: CPU
    force_render: bool

    def __init__(self, cpu: CPU) -> None:
        self.cpu = cpu
        self.force_render = True

    def should_render(self, events: list[pygame.event.Event]) -> bool:
        if self.force_render:
            self.force_render = False
            return True

        should_render = False
        for event in events:
            if event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos

                for x in range(8):
                    x0 = MARGIN + PIXSIZE * x
                    for y in range(8):
                        y0 = MARGIN + PIXSIZE * y
                        if x0 <= mx < x0 + PIXSIZE and y0 <= my < y0 + PIXSIZE:
                            self.cpu.screen[y][x] ^= 1
                            should_render = True

        return should_render

    def render(self) -> None:
        pygame.draw.rect(
            screen,
            BORDERCOL,
            (MARGIN - 2, MARGIN - 2, PIXSIZE * 8 + 3, PIXSIZE * 8 + 3),
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


class RegistersSprite(Sprite):
    """
    Sprite for the registers and input port sections of the cpu.
    """

    cpu: CPU
    force_render: bool

    def __init__(self, cpu: CPU) -> None:
        self.cpu = cpu
        self.force_render = True

    def should_render(self, events: list[pygame.event.Event]) -> bool:
        if self.force_render:
            self.force_render = False
            return True

        should_render = False
        for event in events:
            if event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos

                for i in range(9):
                    x0 = MARGIN * (2 + i) + PIXSIZE * (8 + i)
                    for y in range(8):
                        y0 = MARGIN + PIXSIZE * y
                        if x0 <= mx < x0 + PIXSIZE and y0 <= my < y0 + PIXSIZE:
                            bit = 1 << (7 - y)
                            if i == 8:
                                self.cpu.port ^= bit
                            else:
                                self.cpu.registers[i] ^= bit
                            should_render = True

        return should_render

    def render(self) -> None:
        # registers
        x = MARGIN * 2 + PIXSIZE * 8
        for i in range(8):
            pygame.draw.rect(
                screen,
                BORDERCOL,
                (x - 2, MARGIN - 2, PIXSIZE + 4, PIXSIZE * 8 + 3),
            )

            display_bin_value(
                f'r{i}', self.cpu.registers[i], x, MARGIN * 2 + PIXSIZE * 8
            )
            x += PIXSIZE + MARGIN

        # input port
        display_bin_value(f'port', self.cpu.port, x, MARGIN * 2 + PIXSIZE * 8)


class RunSprite(Sprite):
    """
    Sprite for the info and run logic of the cpu.
    """

    cpu: CPU
    run: bool
    prev_step: float | None  # timestamp in seconds if run, None otherwise
    force_render: bool

    def __init__(self, cpu: CPU) -> None:
        self.cpu = cpu
        self.run = False
        self.prev_step = None
        self.force_render = True

    def should_render(self, events: list[pygame.event.Event]) -> bool:
        if self.force_render:
            self.force_render = False
            return True

        should_render = False

        for event in events:
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_s:
                    self.cpu.step()
                    should_render = True
                elif event.key == pygame.K_SPACE:
                    self.run = not self.run
                    self.prev_step = None
                    should_render = True
                elif event.key == pygame.K_r:
                    self.cpu.reset()
                    should_render = True

        if self.run:
            timestamp = pygame.time.get_ticks() / 1000
            if self.prev_step is None or timestamp - self.prev_step > RUNDELAY:
                self.prev_step = timestamp
                self.cpu.step()
                should_render = True

                if self.cpu.get_instruction()[0] == OpcodeEnum.HLT:
                    self.run = False

        return should_render

    def render(self) -> None:
        _, b, a = self.cpu.get_instruction()
        ra, rb = self.cpu.registers[a], self.cpu.registers[b]

        texts = [
            f'PC: {self.cpu.pc}/63',
            f'Flags: Z = {self.cpu.flag_Z}, C = {self.cpu.flag_C}',
            f'Values cache: ra = {ra}, rb = {rb}',
            f'Instruction to run: {self.cpu.prettyprint_instruction()}',
            'Commands: <S> step, <Space> toggle run, <R> reset',
        ]
        y = MARGIN * 3 + PIXSIZE * 8 + FONTSIZE
        for text in texts:
            screen.blit(font.render(text, True, FGCOL), (MARGIN, y))
            y += FONTSIZE


def display_bin_value(name: str, value: int, x: int, y: int) -> None:
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

        should_render = False
        for sprite in sprites:
            should_render |= sprite.should_render(events)

        if should_render:
            screen.fill(BGCOL)
            for sprite in sprites:
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
screen = pygame.display.set_mode(
    (MARGIN * 11 + PIXSIZE * 17, MARGIN * 4 + PIXSIZE * 8 + FONTSIZE * 6)
)
font = pygame.font.SysFont('monospace', FONTSIZE, bold=True)

cpu = CPU(instructions)
screen_sprite = ScreenSprite(cpu)
registers_sprite = RegistersSprite(cpu)
run_sprite = RunSprite(cpu)

if __name__ == '__main__':
    main([screen_sprite, registers_sprite, run_sprite])

# TODO "debugging symbols", store original program lines in comments and display
