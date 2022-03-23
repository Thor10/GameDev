import pygame, sys
from pygame.locals import * 

pygame.init()
pygame.font.init()

fonts = {}
def text(text, color, size, x, y, font):
    if size not in fonts:
        fonts[size] = pygame.font.Font(font, size)
    screen.blit(fonts[size].render(text, False, color), (x, y))

pygame.mouse.set_visible(False)

size = (1000, 720)

screen = pygame.display.set_mode(size, RESIZABLE)
pygame.display.set_caption("Pygame Projects 2021")

fps = pygame.time.Clock()
font = "m6x11.ttf"

r = 0 
g = 0
b = 0

increase = 0.75

add = False

begin_pos = 160

#############
#############
############# Modular-ish code
#############   Easy to change for each year
#############
#############
projects_made_using = {
}



projects = [ 
    "Pixel Art Maker: ",
    "L0CKED: ",
    "Click & Destroy: ",
    "Astober: ",
    "G0LD: ",
    "Ru5h: ",
    "COGE: ",
]

projects_description = [ 
    "small program to make pixel art, ",
    "one of the best games I made in the past 2 years, ",
    "a game in which you destroy circles, ",
    "a space dodge-ish game in which you dodge squares, ",
    "testing grounds for my C & SDL2 framework, ",
    "a challenging platformer in which you have to reach the end without dying, ",
    "a game engine I am making in C & OpenGL"
]

for i in range(0, len(projects)):
    if i <= 3:
        projects_made_using[str(i)] = "Made Using Pygame & Python"
    if i >= 4 and i <= 6:
        projects_made_using[str(i)] = "Made Using SDL2 & C"

year = 2021


while True:

    r += increase
    g += increase
    b += increase


    if add: 
        increase = 0.75

    else:
        increase = 0

    if r >= 255:
        r = 255
        add = False

    if r <= 0:
        add = True

    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
        if event.type == KEYDOWN:
            add = True

    screen.fill((0, 0, 0))

    text("Yoooo! Today I am going to be showing off some of my best game dev projects I made in " + str(year) + " ...", (r, g, 230), 20, 23, 50, font)
    text("These projects were made using C or Python", (r, g, 230), 20, 23, 80, font)
    text("With C I used either SDL2 & OpenGL", (r, g, 230), 20, 23, 110, font)
    text("With Python I just used Pygame", (r, g, 230), 20, 23, 140, font)

    for i in range(1, len(projects)):
        text(projects[i - 1], (r, 120, 255), 20, 40, begin_pos + (30 * i), font)
        text(projects_description[i - 1] + projects_made_using[str(i - 1)], (150, 130, 255), 20, 43 + 9 * len(projects[i - 1]), begin_pos + (30 * i), font)

    text("Credits to Daniel Linsson for the Font", (r, 210, b), 40, pygame.display.get_surface().get_size()[0] / 2 - 350, 500, font)

    pygame.display.update()
    fps.tick(140)

