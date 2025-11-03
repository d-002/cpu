from pygame.locals import *
import pygame

from theme import Theme
from settings import *
from utils import to_hex, to_bin

def border_area(screen, rect, fill):
    border = pygame.Rect(rect.x - Theme.border, rect.y - Theme.border,
                         rect.w + 2 * Theme.border, rect.h + 2 * Theme.border)
    pygame.draw.rect(screen, Theme.cols['border'], border)
    pygame.draw.rect(screen, fill, rect)

def display(screen, data):
    screen.fill(Theme.cols['bg'])

    # rom
    rom_rows = 6
    rom_rect = pygame.Rect(Theme.margin, Theme.margin,
                           (3 * ROM_CACHE - 1) * Theme.font_width,
                           rom_rows * Theme.font_height)
    border_area(screen, rom_rect, Theme.cols['white'])
    y = min(max(0, int(data.program_counter / ROM_CACHE)), ROM - rom_rows)
    for dy in range(rom_rows):
        line = ' '.join(to_hex(data.rom[x + (y + dy) * ROM_CACHE])
                               for x in range(ROM_CACHE))
        screen.blit(Theme.font.render(line, True, Theme.cols['black']),
                    (rom_rect.x, rom_rect.y + dy * Theme.font_height))
