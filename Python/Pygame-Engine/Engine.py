import pygame as pg
import random, math, sys

# Using Python 3.9.+ and Pygame == 2.1.2

pg.init()
pg.font.init()
pg.mixer.init()
pg.display.init()

class pe_win:
    def __init__(self, width = 640, height = 480, caption = "pg game", flags = None, icon = None):
        self.size = (width, height)
        self.caption = caption
        self.flags = flags
        self.icon = icon
        self.screen = pg.display.set_mode(self.size, self.flags)

        pg.display.set_caption(caption)

        self.fps = pg.time.Clock()

        if icon is not None:
            pg.display.set_icon(pg.image.load(self.icon).convert())
    
        self.display_size = pg.display.get_surface().get_size()

    def cls(self, color = (0, 0, 0)):
        self.screen.fill(color)

    def get_current_size(self):
        self.display_size = pg.display.get_surface().get_size()
        return self.display_size


    def draw(self, main_surf=None, pos=[0, 0], scale=0, fps=60):
        self.screen.fill((0, 0, 0))

        self.display_size = pg.display.get_surface().get_size()
        if main_surf is not None and scale:
            pg.transform.scale(main_surf, self.display_size, dest_surface=self.screen)

        if main_surf is not None and not scale:
            self.screen.blit(main_surf, pos)

        pg.display.update()
        self.fps.tick(fps)

    def close(self):
        pg.quit()
        sys.exit()


class pe_mouse:
    def __init__(self, img = None, surf = None, visible = True):
        self.visible = visible

        self.img = img 
        self.surf = surf 

        pg.mouse.set_visible(self.visible)
        self.pos = pg.mouse.get_pos()

    def get_scaled_coords(self, win_size, surf_size):
        self.pos = pg.mouse.get_pos()
        return (self.pos[0] / (win_size[0] / surf_size[0]), self.pos[1] / (win_size[1] / surf_size[1]))

    def draw_mouse_img(self, scaled, win, surf_size):
        if scaled:
            coords_r = self.get_scaled_coords(win.get_current_size(), surf_size)
            coords = [coords_r[0], coords_r[1]]
            size = [self.img.get_width(), self.img.get_height()] 
            coords[0] -= size[0] / 2
            coords[1] -= size[1] / 2

            self.surf.blit(self.img, pg.Rect(coords[0], coords[1], size[0], size[1]))

        else:
            coords = [self.pos[0], self.pos[1]]
            size = [self.img.get_width(), self.img.get_height()] 
            coords[0] -= size[0] / 2
            coords[1] -= size[1] / 2

            self.surf.blit(self.img, pg.Rect(coords[0], coords[1], size[0], size[1]))


    def update(self):
        self.pos = pg.mouse.get_pos()

    def draw(self, scaled = None, win = None):
        self.draw_mouse_img(scaled, win, win.get_size())

class pe_img:
    def __init__(self, file, x, y):
        self.file = file
        self.img = pg.image.load(file).convert()
        self.rect = pg.Rect(x, y, self.img.get_width(), self.img.get_height())
    
    def swap_col(self, old_c, new_c):
        img_copy = pg.Surface((self.rect.w, self.rect.h))
        img_copy.fill(new_c)
        self.image.set_colorkey(old_c)
        img_copy.blit(self.image, (0, 0))
        self.image = img_copy

    def rotated_draw(self, surf, angle):
        topleft = self.rect.topleft
        rotated_image = pg.transform.rotate(self.img, angle)
        # self.img.set_colorkey(colorkey)
        # set color key on image to 255 from default
        new_rect = rotated_image.get_rect(center = self.img.get_rect(topleft = topleft).center)
        surf.blit(rotated_image, new_rect.topleft)        

    def draw(self, surf):
        surf.blit(self.img, self.rect)

    def draw_rect(self, surf):
        pg.draw.rect(surf, (220, 240, 167), self.rect)

class pe_circle:
    def __init__(self, surface, color, pos, radius):
        self.surface = surface
        self.color = color
        self.pos = pos
        self.radius = radius

    def draw(self):
        pg.draw.circle(self.surface, (255, 255, 255), self.pos, self.radius + 1)
        pg.draw.circle(self.surface, self.color, self.pos, self.radius)  

    def collision(self, otherCircle):
        xDif = self.pos[0] - otherCircle.pos[0]
        yDif = self.pos[1] - otherCircle.pos[1]

        distance = math.sqrt((xDif ** 2) + (yDif ** 2))

        if distance < self.radius:
            return True


class pe_button:
    def __init__(self, width, height, x, y, color = (220, 240, 210)):
        self.x = x
        self.y = y
        self.color = color
        self.rect = pg.Rect(x - width / 2, y, width, height)

    def draw(self, surf):
        pg.draw.rect(surf, self.color)

    def on_hover(self, pos, call_func, new_func):
        if self.rect.collidepoint((pos[0], pos[1])):
            call_func(self)

        else:
            new_func(self)


    def on_click(self, pos, call_func, new_func, mouse_down):
        if self.rect.collidepoint((pos[0], pos[1])) and mouse_down:
            call_func(self)

        else:
            new_func(self)


class pe_grid:
    def __init__(self, w, h, win_size):
        self.grid = [[(-1, -1) for i in range(int(w))] for row in range(int(h))]
        self.size = (w, h)
        self.tile_size = [(win_size[0]) / w, (win_size[1]) / h]

    def set_at(self, x, y, val):
        if x < self.size[0] + 1 and y < self.size[1]:
            self.grid[y][x] = val

    def draw(self, surf, grid_update, draw_it = 1):
        w, h = surf.get_size()
        grid = self.grid
        y = 0
        if draw_it:
            pg.draw.line(surf, (128, 128, 128), (w, 0), (w, h), width=3)
            pg.draw.line(surf, (128, 128, 128), (0, h), (w, h), width=3)
        for row in grid:
            x = 0
            for cell in row:
                rect = pg.Rect(x * self.tile_size[0], 
                               y * self.tile_size[1], 
                               self.tile_size[0], 
                               self.tile_size[1])

                grid_update(cell, rect, x, y)
                if draw_it:
                    pg.draw.lines(surf, (128, 128, 128), True, [(rect.x, rect.y), 
                                                            (rect.x, h + self.tile_size[1]), 
                                                            (rect.x, rect.y), 
                                                            (w + self.tile_size[0], rect.y)])

                x += 1
                
            y += 1






def pe_outline_v0(image, loc, surf):
    mask = pg.mask.from_surface(image)
    mask_outline = mask.outline()

    n = 0

    for point in mask_outline:
        mask_outline[n] = (point[0] + loc[0], point[1] + loc[1])
        n += 1

    pg.draw.polygon(surf, (255, 255, 255), mask_outline, 2)

def pe_outline_v1(image, loc, surf):
    mask = pg.mask.from_surface(image)
    mask_surf = mask.to_surface()

    mask_surf.set_colorkey((0, 0, 0))

    surf.blit(mask_surf, (loc[0] - 1, loc[1]))
    surf.blit(mask_surf, (loc[0] + 1, loc[1]))
    surf.blit(mask_surf, (loc[0], loc[1] - 1))
    surf.blit(mask_surf, (loc[0], loc[1] + 1))

def pe_outline_v2(image, loc, surf):
    mask = pg.mask.from_surface(image)
    mask_outline = mask.outline()

    mask_surf = pg.Surface(image.get_size())

    for pixel in mask_outline:
        mask_surf.set_at(pixel, (255, 255, 255))

    mask_surf.set_colorkey((0, 0, 0))

    surf.blit(mask_surf, (loc[0] - 1, loc[1]))
    surf.blit(mask_surf, (loc[0] + 1, loc[1]))
    surf.blit(mask_surf, (loc[0], loc[1] - 1))
    surf.blit(mask_surf, (loc[0], loc[1] + 1))



def dist2d(x0, y0, x1, y1):
    return math.sqrt(((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)))
