from Engine import *
import pygame as pg
import json


"""
rectangular marquee tool
brush thickness
layers
frames
rgb custom colors
paint bucket / fill tool
line drawing algorithm 
undo/redo
for opening a existing one just load the image and edit that surface
"""


size = (640, 480) 
window = pe_win(size[0], size[1], flags = pg.RESIZABLE)
display = pg.Surface((480, 480))

pg.display.set_caption("PxArt - DevHedron")
pg.display.set_icon(pg.image.load("./img/icon.png"))

grid = pe_grid(16, 16, display.get_size())

mc_pos = [0, 0]

mouse = pe_mouse(surf = display, visible = True)
mouse_lbtn_down = 0
mouse_rbtn_down = 0

colors = json.loads(open("./json/color_vals.json", "r").read())
m_colors = json.loads(open("./json/colors.json", "r").read())
buttons = json.loads(open("./json/keys.json", "r").read())

current_color = colors["WHITE"]

saving = 0

mouse_surf = pg.Surface((grid.tile_size[0], grid.tile_size[1]))
mouse_surf.set_alpha(100)

def grid_update(i, rect, x, y):
    global display, mc_pos, grid, colors, m_colors, saving
    if i[0] == 2:
        for color, color_val in colors.items():
            if i[1] == color_val:
                pg.draw.rect(display, m_colors[color], rect)

    if x == mc_pos[1] and y == mc_pos[0] and not saving:
        for color, color_val in colors.items():
            if i[1] == color_val:
                mouse_surf.fill(m_colors[color])
                break

        display.blit(pg.transform.scale(mouse_surf, (rect.w, rect.h)), rect)
    

sizes = [
    pg.image.load("./img/8.png").convert(),
    pg.image.load("./img/16.png").convert(),
    pg.image.load("./img/32.png").convert(),
    pg.image.load("./img/64.png").convert(),
]

size_s = [8, 16, 32, 64]

rects = []

y = window.screen.get_size()[1] / 2 - 225
for i in range(0, len(sizes)):
    sizes[i] = pg.transform.scale(sizes[i], (300, 100))
    rects.append(pg.Rect(window.screen.get_size()[0] / 2 - 150, y, 300, 100))
    y += 110
    

draw_grid = 1
running = 1
menu = 1
drawing = 0
color_pick = 0

curr_size = []

draw_posx = size[0] / 2 - display.get_size()[0] / 2
draw_posy = size[1] / 2 - display.get_size()[1] / 2

while running:
    while menu:
        for e in pg.event.get():
            if e.type == pg.QUIT:
                menu = 0
                drawing = 0
                running = 0

            if e.type == pg.MOUSEBUTTONDOWN:
                for i in range(0, len(rects)):
                    if (pg.Rect(pg.mouse.get_pos()[0], pg.mouse.get_pos()[1], 16, 16)).colliderect(rects[i]):
                        grid = pe_grid(size_s[i], size_s[i], display.get_size())
                        curr_size = [size_s[i], size_s[i]]
                        drawing = 1
                        menu = 0

        for i in range(0, len(sizes)):
            window.screen.blit(sizes[i], rects[i])

        pg.display.update()

    while drawing:
        ## events
        for e in pg.event.get():
            if e.type == pg.QUIT:
                menu = 0
                running = 0
                drawing = 0

            if e.type == pg.KEYDOWN:
                if e.key == pg.K_g:
                    draw_grid = not draw_grid

                if e.key == pg.K_ESCAPE:
                    menu = 1
                    drawing = 0

                if e.key == pg.K_e:
                    mouse_rbtn_down = not mouse_rbtn_down

                if e.key == pg.K_RSHIFT:
                    saving = 1
                    draw_grid = 0

                if e.key == pg.K_EQUALS:
                    display = pg.transform.scale(display, (display.get_size()[0] + 10, display.get_size()[1] + 10))
                    backup = grid
                    grid = pe_grid(backup.size[0], backup.size[1], display.get_size())
                    grid.grid = backup.grid


                if e.key == pg.K_MINUS:
                    display = pg.transform.scale(display, (display.get_size()[0] - 10, display.get_size()[1] - 10))
                    backup = grid
                    grid = pe_grid(backup.size[0], backup.size[1], display.get_size())
                    grid.grid = backup.grid

                if e.key == pg.K_RIGHT:
                    draw_posx += 10
    
                if e.key == pg.K_LEFT:
                    draw_posx -= 10

                if e.key == pg.K_UP:
                    draw_posy -= 10

                if e.key == pg.K_DOWN:
                    draw_posy += 10

                if e.key == pg.K_a:
                    color_pick = 1

                if e.key == pg.K_c:
                    backup = grid
                    grid = pe_grid(backup.size[0], backup.size[1], display.get_size())

                for i in buttons.keys():
                    if str(e.key) == i:
                        current_color = colors[buttons[i]]
                    

            if e.type == pg.KEYUP:
                if e.key == pg.K_RSHIFT:
                    saving = 0
                    draw_grid = 1

            if e.type == pg.MOUSEBUTTONDOWN:
                if e.button == 1:
                    mouse_lbtn_down = 1 
                else: 
                    mouse_rbtn_down = 1

            if e.type == pg.MOUSEBUTTONUP:
                if e.button == 1:
                    mouse_lbtn_down = 0
                else:
                    mouse_rbtn_down = 0

        ## updating
         
        m_pos = list(pg.mouse.get_pos())
        # take into account display size somehow
        m_pos[0] -= draw_posx 
        m_pos[1] -= draw_posy

        # problem with filling the already filled, use different numb
        if not saving:
            # flipped coords
            if m_pos[0] >= 0 and m_pos[1] >= 0:
                mc_pos = [int(m_pos[1] // grid.tile_size[1]), int(m_pos[0] // grid.tile_size[0])] 
                try:
                    if (mc_pos[1] < grid.size[0] + 1 and mc_pos[0] < grid.size[1]):
                        if grid.grid[mc_pos[0]][mc_pos[1]][0] != 2:
                            if saving == 0:
                                grid.grid[mc_pos[0]][mc_pos[1]] = (0, current_color)

                    if mouse_lbtn_down == 1 and color_pick == 0:
                        grid.set_at(mc_pos[1], mc_pos[0], (2, current_color)) 

                    if mouse_lbtn_down == 1 and color_pick == 1:
                        col = display.get_at((int(m_pos[0]), int(m_pos[1])))
                        print(m_pos)
                        print(col)
                        color_pick = 0

                    if mouse_rbtn_down == 1:
                        grid.set_at(mc_pos[1], mc_pos[0], (1, 1)) 

                except Exception as e:
                    print(e)


        ## drawing  
        display.fill((100, 100, 100))
        grid.draw(display, grid_update, draw_grid)
        window.draw(display, [draw_posx, draw_posy], 0, 120)

        if saving:
            pg.image.save(pg.transform.scale(display, grid.tile_size), "art.png")

window.close()
