import pygame
from pygame.locals import *
pygame.init()

from theme import Theme
from data import Data
from layout import display

FPS = 60

data = Data()

screen = pygame.display.set_mode((Theme.W, Theme.H))
clock = pygame.time.Clock()

prev = 0
run = True
while run:
    events = pygame.event.get()
    for event in events:
        if event.type == QUIT:
            run = False
        if event.type == KEYDOWN and event.key == K_ESCAPE:
            run = False

    display(screen, data)

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
