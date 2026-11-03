import sys
import pygame
from os.path import isfile

FPS = 60


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
clock = pygame.time.Clock()
ticks = pygame.time.get_ticks()
screen = pygame.display.set_mode((640, 480))
font = pygame.font.SysFont('monospace', 12)

if __name__ == '__main__':
    main()
