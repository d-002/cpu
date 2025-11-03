import pygame
from pygame.locals import *

from settings import *

class Theme:
    W = 900
    H = 500
    margin = 40
    border = 4

    lamps = [pygame.image.load('lamp_off.png'),
                  pygame.image.load('lamp_on.png')]

    cols = {
            'bg': (20, 20, 30),
            'border': (100, 100, 120),
            'primary': (255, 255, 255),
            'secondary': (200, 200, 220),
            'black': (0, 0, 0),
            'white': (255, 255, 255)}

    font_height = 24
    font = pygame.font.SysFont('consolas, monospace', font_height, bold=True)
    font_width = font.render(' ', False, cols['black']).get_width()
