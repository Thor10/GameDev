from Engine import *
import pygame as pg
import json


"""
rectangular marquee tool
pencil
eraser
colors
color picker
zoom in out
layers
frames
rgb custom colors
color palettes
move
paint bucket
line 
rectangle
lighten/darken tool
undo/redo

"""


size = (640, 640)
window = pe_win(size[0], size[1], flags = 0)
display = pg.Surface((480, 480))

pg.display.set_caption("PxArt - DevHedron")

grid = pe_grid(16, 16, display.get_size())

mc_pos = [0, 0]

mouse = pe_mouse(surf = display, visible = True)
mouse_lbtn_down = 0
mouse_rbtn_down = 0



colors = json.loads(open("color_vals.json", "r").read())
m_colors = json.loads(open("colors.json", "r").read())
buttons = json.loads(open("keys.json", "r").read())

current_color = colors["WHITE"]

saving = 0

def grid_update(i, rect, x, y):
    global display, mc_pos, grid, colors, m_colors, saving
    if i[0] == 2:
        for color, color_val in colors.items():
            if i[1] == color_val:
                pg.draw.rect(display, m_colors[color], rect)

    if x == mc_pos[1] and y == mc_pos[0] and not saving:
        pg.draw.rect(display, (150, 150, 150), rect)
    

sizes = [
            pg.image.load("./8.png").convert(),
            pg.image.load("./16.png").convert(),
            pg.image.load("./32.png").convert(),
            pg.image.load("./64.png").convert(),

        ]




size_s = [8, 16, 32, 64]

rects = []

y = 320 - 225
for i in range(0, len(sizes)):
    sizes[i] = pg.transform.scale(sizes[i], (300, 100))
    rects.append(pg.Rect(320 - 150, y, 300, 100))
    y += 120
    

draw_grid = 1
running = 1
menu = 1
drawing = 0
fill = 0



curr_size = []
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

                if e.key == pg.K_f:
                    fill = not fill

                if e.key == pg.K_RSHIFT:
                    saving = 1
                    draw_grid = 0

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
        m_pos = mouse.get_scaled_coords(window.screen.get_size(), display.get_size())


        # problem with filling the already filled, use different numb
        if not saving:
            # flipped coords
            mc_pos = [int(m_pos[1] // grid.tile_size[1]), int(m_pos[0] // grid.tile_size[0])] 
            if (mc_pos[1] < grid.size[0] + 1 and mc_pos[0] < grid.size[1]):
                if grid.grid[mc_pos[0]][mc_pos[1]][0] != 2:
                    if saving == 0:
                        grid.grid[mc_pos[0]][mc_pos[1]] = (0, current_color)

            if mouse_lbtn_down == 1:
                grid.set_at(mc_pos[1], mc_pos[0], (2, current_color)) 


            if mouse_lbtn_down == 1 and fill:
                if mc_pos[1] < grid.size[0] + 1 and mc_pos[0] < grid.size[1]:
                    stored_idx = []
                    count = 0
                    for i in range(0, len(grid.grid[mc_pos[0]])):
                        if grid.grid[mc_pos[0]][i][0] > 0 and grid.grid[mc_pos[0]][i][1] != -1:
                            stored_idx.append(i)
                            count += 1

                    if count > 1:
                        for i in range(stored_idx[0], stored_idx[1]):
                            grid.set_at(i, mc_pos[0], (2, current_color))


            if mouse_rbtn_down == 1:
                grid.set_at(mc_pos[1], mc_pos[0], (1, 1)) 

        ## drawing  
        display.fill((100, 100, 100))

        grid.draw(display, grid_update, draw_grid)

        window.draw(display, [0, 0], 1, 120)
        
        if saving:
            pg.image.save(window.screen, "art.png")

window.close()
