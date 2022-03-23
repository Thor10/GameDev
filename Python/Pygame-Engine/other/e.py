# help(pygame.module)
# Get by getting the function and then doing
# Only seet variable

# Organize Setters, Getters, and Update

class Game:
    pass

class Particles:
    def __init__(self):
        pass


class Projectile:
    def __init__(self):
        pass

# class AnimatedImage:
# class SmallerSuface:
# class Layers
# class Game_Obj:
    # Rect
    # Image
# class Colors:
# class Map:
# class Menu:
# class Text:
# class Maths:


# global numbers
# numbers = {}
# for num in range(10):
#     img = pygame.image.load('data/numbers/' + str(num) + '.png').convert()
#     numbers[str(num)] = img.copy()


########################
###########             Work
########################

def draw_number(number,x,y,surface):
    global numbers
    x_offset = 0
    for char in number:
        surface.blit(numbers[char],(x+x_offset,y))
        x_offset += 23

# e.load_particle_images('data/particles')

# bg_img = pygame.image.load('data/bg.png').convert()
# bg_img.set_colorkey((0,0,0))

# instructions_img = pygame.image.load('data/instructions.png').convert()
# instructions_img.set_colorkey((0,0,0))

def advance(pos,rot,amt):
    x = pos[0] + math.cos(math.radians(rot)) * amt
    y = pos[1] + math.sin(math.radians(rot)) * amt
    return [x,y]

def normalize(num,amt):
    if num < -amt:
        num += amt
    elif num > amt:
        num -= amt
    else:
        num = 0
    return num

def mirror_angle(original,base):
    dif = 180-base
    base = 180
    new = original+dif
    new = new % 360
    dif = base-new
    return original + dif * 2

def rotate_towards(target,original,rate):
    dif = 180-target
    target = 180
    new = original+dif
    new = new % 360
    dif = target-new
    if abs(dif) < rate:
        return dif
    else:
        return dif/abs(dif) * rate

def clip(surf,x,y,x_size,y_size):
    clipR = pygame.Rect(x,y,x_size,y_size)
    surf.set_clip(clipR)
    image = surf.subsurface(surf.get_clip())
    return image.copy()

history_length = 60

last_tick = 0
frame_data = []
for i in range(history_length):
    frame_data.append(60)

def get_time():
    global last_tick
    t = pygame.time.get_ticks()
    time_diff = t - last_tick
    last_tick = t
    return time_diff

def get_framerate():
    global frame_data
    time_diff = get_time()
    try:
        time_fps = 1000/time_diff
    except ZeroDivisionError:
        time_fps = 1000
    frame_data.append(time_fps)
    frame_data.pop(0)
    return sum(frame_data) / len(frame_data)

def load_tileset(path):
    tileset_img = pygame.image.load(path + 'tileset.png').convert()
    tileset_img.set_colorkey((0, 0, 0))
    width = tileset_img.get_width()
    tile_size = [16, 16]
    tile_count = int((width + 1) / (tile_size[0] + 1))
    images = [clip(tileset_img, i * (tile_size[0] + 1), 0, tile_size[0], tile_size[1]) for i in range(tile_count)]
    return images


radius = random.randint(1, 100)
time_count = 0
materials = {}

def survive(player_mats, string):
    sentence = " ".join(player_mats)
    new_sentence = sentence.split(" ")

    count = 0
    index = 0
    animal_count = 0

    for i in new_sentence:
        count += 1
        if i == string:
            index = count - 2
            animal_count = new_sentence[index]

    return animal_count

def explore(radius, time_count, materials):
    diameter = radius * 2
    circumference = 3.14 * diameter
    area = 3.14 * (radius**2)

    if circumference < 50 and area < 50:
        time_count += 0

    if circumference > 50 and area > 50:
        time_count += 1

    materials = {
        "wood" : random.randint(210, 300),
        "leaves" : random.randint(210, 400),
        "fresh-water" : str(random.randint(10, 50)) + " Liters",
        "animals" : random.randint(8, 15) + 2
    }

    return time_count, materials

def harvest(materials, material):
    materials[material] = 0

    return materials[material]

island = list(explore(radius, time_count, materials))


console_arrow_rect_x, console_arrow_rect_y = 500, 20
console_text_rect_x, console_text_rect_y = console_arrow_rect_x + 10, 20

commands = {"help" : "explore, harvest <material>, build raft, show-mats, time, eat, hunt, goal, escape(only works after making raft)"}


# Text Box
# Runtim Debugger
# Distance finder
# Average
# Direction from one point to another
def load_map(map_id):
    map_img = pygame.image.load('data/maps/' + map_id + '.png')
    map_data = []
    enemy_count = 0
    for y in range(map_img.get_height()):
        for x in range(map_img.get_width()):
            c = map_img.get_at((x, y))
            if c[1] == 100:
                map_data.append({'type': 'grass', 'pos': [x, y], 'h_pos': 400, 'enemy': False})
            if c[1] == 255:
                map_data.append({'type': 'bush', 'pos': [x, y], 'h_pos': 400, 'enemy': False})
            if c[2] == 100:
                map_data.append({'type': 'rock', 'pos': [x, y], 'h_pos': 400, 'enemy': False})
            if c[2] == 255:
                map_data.append({'type': 'box', 'pos': [x, y], 'h_pos': 400, 'enemy': False})
            if c[0] == 255:
                spawn = [x, y]
            if c[0] == 100:
                map_data[-1]['enemy'] = True
                enemy_count += 1
    return map_data, spawn, enemy_count


def ColGen():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

# Vectors

# gun_rect = (player_rect.centerx - 32 - scroll[0], player_rect.centery - scroll[1])
# direction = math.atan2(my - gun_rect[1], mx - gun_rect[0])

# bx += math.cos(direction) * 20
# by += math.sin(direction) * 20

# Dirrection to

def load_map(path):
    return [list(row) for row in open(path + ".txt", "r").read().split("\n")]
"""
tile_index = {
                1: tile,
                2: tile_2,
                3: tile,
                4: enemy_follow
             }"""

fonts = {}
def text(text, color, size, x, y, font, surface):
    if size not in fonts:
        fonts[size] = pygame.font.Font(font, size)

    surface.blit(fonts[size].render(text, False, color), (x, y))

def collision_test(rect, tiles):
    return [tile for tile in tiles if rect.colliderect(tile)]

def move(rect, movement, tiles):
    collision_types = {
        "top"     : False,
        "bottom"  : False,
        "right"   : False,
        "left"    : False
    }

    rect.x += movement[0]
    # hit_list = collision_test(rect, tiles)
    hit_list = [tile for tile in tiles if rect.colliderect(tile)]

    for tile in hit_list:
        if movement[0] > 0:
            rect.right = tile.left 
            collision_types["right"] = True

        elif movement[0] < 0:
            rect.left = tile.right
            collision_types["left"] = True

    rect.y += movement[1]
    hit_list = [tile for tile in tiles if rect.colliderect(tile)]

    for tile in hit_list:
        if movement[1] > 0:
            rect.bottom = tile.top
            collision_types["bottom"] = True

        if movement[1] < 0:
            rect.top = tile.bottom 
            collision_types["top"] = True

    return rect, collision_types

global animation_frames
animation_frames = {}

def update_score(score, high_score):
    if score > high_score:
        high_score = score

    return high_score


def load_animation(path, frame_durations, animation_name):
    global animation_frames

    animation_frame_data = []

    n = 0

    for frame in frame_durations:
        animation_frame_id = animation_name + "_" + str(n)
        img_loc = path + "/" + animation_frame_id + ".png"
        animation_image = pygame.image.load(img_loc).convert()
        animation_frames[animation_frame_id] = animation_image.copy()

        for i in range(frame):
            animation_frame_data.append(animation_frame_id)


        n += 1

    return animation_frame_data

def change_action(action_var, frame, new_val):
    if action_var != new_val:
        action_var = new_val
        frame = 0

    return action_var, frame

def MatrixMultiply(cords, matrix):
    cords = [cords[0] * 32, cords[1] * 32] # 32 = tileSize
    return [(cords[0] * matrix[0][0]) + (cords[1] * matrix[1][0]),
            (cords[0] * matrix[0][1]) + (cords[1] * matrix[1][1])]

######
######
######

def sprites(image_path, num_of_images):
    image_list = [pygame.image.load(image_path + str(i) + '.png').convert_alpha() for i in range(num_of_images)]
    return image_list

def flip_sprites(image_list):
    flipped_sprites = [pygame.transform.flip(a, True, False) for a in image_list]
    return flipped_sprites

def gen_chunk(x, y, chunk_size):
    chunk_data = []
    for y_pos in range(chunk_size):
        for x_pos in range(chunk_size):
            target_x = x * chunk_size + x_pos
            target_y = y * chunk_size + y_pos

            tile_type = 0

            if target_y > 10:
                tile_type = 2

            elif target_y == 10:
                tile_type = 1

            elif target_y == 9:
                if random.randint(1, 5) == 1:
                    tile_type = 3

            if tile_type != 0:
                chunk_data.append([[target_x, target_y], tile_type])

    return chunk_data

def draw_bg(bg, bg_x, bg_y, screen_width):
    screen.blit(bg, (bg_x, bg_y))
    screen.blit(bg, (bg_x + screen_width, bg_y))
