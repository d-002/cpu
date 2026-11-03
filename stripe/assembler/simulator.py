import sys
import pygame
from os.path import isfile

FPS = 60
BGCOL = (21, 24, 25)
FGCOL = (217, 220, 221)
BORDERCOL = (63, 63, 63)
LIGHT_ON = (237, 199, 145)
LIGHT_OFF = (41, 32, 26)
MARGIN = 15
PIXSIZE = 30
FONTSIZE = 16


class CPU:
    instruction_cache: tuple[int, int, int]
    registers: list[int]
    screen: list[list[int]]

    def __init__(self) -> None:
        self.reset()

    def reset(self) -> None:
        self.instruction_cache = (0, 0, 0)
        self.registers = [0] * 8
        self.screen = [[0] * 8 for _ in range(8)]

        # TODO remove
        for i in range(8):
            self.screen[i][i] = 1

    def display(self, screen: pygame.Surface) -> None:
        # screen
        bordersize = PIXSIZE * 8 + 4
        pygame.draw.rect(
            screen, BORDERCOL, (MARGIN - 2, MARGIN - 2, bordersize, bordersize)
        )
        for y, line in enumerate(self.screen):
            for x, pix in enumerate(line):
                color = LIGHT_ON if pix else LIGHT_OFF
                pygame.draw.rect(
                    screen,
                    color,
                    (
                        MARGIN + PIXSIZE * x,
                        MARGIN + PIXSIZE * y,
                        PIXSIZE,
                        PIXSIZE,
                    ),
                )
        screen.blit(
            font.render('Screen', True, FGCOL),
            (MARGIN, MARGIN * 2 + PIXSIZE * 8),
        )


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

        screen.fill(BGCOL)
        cpu.display(screen)

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
screen = pygame.display.set_mode((640, 480))
font = pygame.font.SysFont('monospace', FONTSIZE, bold=True)

cpu = CPU()

if __name__ == '__main__':
    main()
