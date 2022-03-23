import pygame, sys, random, math, json
from pygame.locals import *

pygame.init()
pygame.mouse.set_visible(True)

size = (640, 480)
screen = pygame.display.set_mode(size, RESIZABLE)
display = pygame.Surface((320, 240))

fps = pygame.time.Clock()

pygame.display.set_caption("Pixel Art")
left_click = False
right_click = False

grid = [[1 for i in range(32)] for row in range(32)]
draw_grid = True
tile_size = (size[0] / 2) / 32 

saving = False



colors = json.loads(open("color_vals.json", "r").read())
m_colors = json.loads(open("colors.json", "r").read())
buttons = json.loads(open("keys.json", "r").read())

my_color = colors["WHITE"]



while True:
	display.fill((0, 0, 0))

	y = 0
	for row in grid:
		x = 0
		for cell in row:
			rect = pygame.Rect(x * tile_size, y * tile_size, tile_size, tile_size)

			for color, color_val in colors.items():
				if cell == color_val:
					pygame.draw.rect(display, m_colors[color], rect)

			if not saving and draw_grid:
				pygame.draw.lines(display, (255, 255, 255), True, [(rect.x, rect.y), (rect.x, size[1]), (rect.x, rect.y), (size[0], rect.y)])

			x += 1
			
		y += 1

	pos = [pygame.mouse.get_pos()[1] / (pygame.display.get_surface().get_size()[1] / (size[1] / 2)), pygame.mouse.get_pos()[0] / (pygame.display.get_surface().get_size()[0] / (size[0] / 2))] # pos[0] is the y and pos[1] is the x

	for color, color_val in colors.items():
		if my_color == color_val:
			pygame.draw.rect(display, m_colors[color], pygame.Rect(pos[1], pos[0], tile_size / 2, tile_size / 2))

	if left_click:
		m_pos = [int(pos[0] // tile_size), int(pos[1] // tile_size)] 
		if m_pos[0] < 33 and m_pos[1] < 32:
			grid[int(pos[0] // tile_size)][int(pos[1] // tile_size)] = my_color

	if right_click:
		m_pos = [int(pos[0] // tile_size), int(pos[1] // tile_size)] 
		if m_pos[0] < 33 and m_pos[1] < 32:
			grid[int(pos[0] // tile_size)][int(pos[1] // tile_size)] = 1
	
	if saving:
		pygame.image.save(pygame.transform.scale(display, (32, 32)), "art.png")

	for event in pygame.event.get():
		if event.type == QUIT:
			pygame.quit()
			sys.exit()

		if event.type == KEYDOWN:
			if event.key == K_s:
				saving = True

			if event.key == K_e:
				grid = [[1 for i in range(32)] for row in range(32)]

			for i in buttons.keys():
				if str(event.key) == i:
					my_color = colors[buttons[i]]
				
			if event.key == K_g:
				if draw_grid:
					draw_grid = False

				else:
					draw_grid = True

		if event.type == KEYUP:
			if event.key == K_s:
				saving = False

		if event.type == MOUSEBUTTONDOWN:
			if event.button == 1:
				left_click = True

			else:
				right_click = True

		if event.type == MOUSEBUTTONUP:
			if event.button == 1:
				left_click = False

			else:
				right_click = False

	screen.blit(pygame.transform.scale(display, pygame.display.get_surface().get_size()), (0, 0))

	pygame.display.update()
	fps.tick(120)
