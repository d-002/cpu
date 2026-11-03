import sys
import pygame
from abc import ABC, abstractmethod
from os.path import isfile

FPS = 60
BGCOL = (21, 24, 25)
FGCOL = (217, 220, 221)
BORDERCOL = (63, 63, 63)
LIGHT_COLS = ((41, 32, 26), (237, 199, 145))
MARGIN = 15
PIXSIZE = 30
FONTSIZE = 16


class Sprite(ABC):
    @abstractmethod
    def should_render(self, events: list[pygame.event.Event]) -> bool:
        """
        Return whether the sprite should be rendered this frame, for example if
        the given events mean its state should change.
        """

    @abstractmethod
    def render(self, screen: pygame.Surface) -> None:
        """
        Render the sprite on the screen.
        """


class CPU(Sprite):
    instruction_cache: tuple[int, int, int]
    registers: list[int]
    screen: list[list[int]]
    force_render: bool

    def __init__(self) -> None:
        self.reset()

    def reset(self) -> None:
        self.instruction_cache = (0, 0, 0)
        self.registers = [0] * 8
        self.screen = [[0] * 8 for _ in range(8)]
        self.force_render = True

        # TODO remove
        for i in range(8):
            self.screen[i][i] = 1
            self.registers[i] = i

    def should_render(self, events: list[pygame.event.Event]) -> bool:
        if self.force_render:
            self.force_render = False
            return True

        return False

    def render(self, screen: pygame.Surface) -> None:
        # screen
        pygame.draw.rect(
            screen,
            BORDERCOL,
            (MARGIN - 2, MARGIN - 2, PIXSIZE * 8 + 4, PIXSIZE * 8 + 4),
        )
        for y, line in enumerate(self.screen):
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
            value = self.registers[i]
            pygame.draw.rect(
                screen,
                BORDERCOL,
                (x - 2, MARGIN - 2, PIXSIZE + 4, PIXSIZE * 8 + 4),
            )

            for y in range(8):
                bit = value & 1
                value >>= 1
                pygame.draw.rect(
                    screen,
                    LIGHT_COLS[bit],
                    (x, MARGIN + (7 - y) * PIXSIZE, PIXSIZE, PIXSIZE - 1),
                )
            screen.blit(
                font.render(f'r{i}', True, FGCOL),
                (x, MARGIN * 2 + PIXSIZE * 8),
            )
            x += PIXSIZE + MARGIN

        # PC and other info
        texts = ['hello world', 'lorem ipsum']
        y = MARGIN * 3 + PIXSIZE * 8 + FONTSIZE
        for text in texts:
            screen.blit(font.render(text, True, FGCOL), (MARGIN, y))
            y += FONTSIZE


def main() -> None:
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
        for sprite in (cpu,):
            if sprite.should_render(events):
                should_render.add(sprite)

        if should_render:
            screen.fill(BGCOL)
            for sprite in should_render:
                sprite.render(screen)

        pygame.display.flip()
        clock.tick(FPS)


if len(sys.argv) != 2:
    print(f'Invalid number of arguments: Expected 1, got {len(sys.argv) - 1}')
    exit(1)

bin_file = sys.argv[1]
if not isfile(bin_file) or not bin_file.endswith('bin'):
    print(f'File "{bin_file}" is not a valid Stripe .bin file')
    exit(1)

pygame.init()
pygame.display.set_caption('Sprite simulator')
clock = pygame.time.Clock()
ticks = pygame.time.get_ticks()
screen = pygame.display.set_mode((MARGIN * 10 + PIXSIZE * 16, 480))
font = pygame.font.SysFont('monospace', FONTSIZE, bold=True)

cpu = CPU()

if __name__ == '__main__':
    main()
