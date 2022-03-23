CC = gcc
uINCLUDE = -lSDL2 -lSDL2_image -lSDL2_ttf
wINCLUDE = -lSDL2main -lSDL2 -lmingw32 
CFLAGS = -Wall -O3 -std=c99 -o Game -mwindows 

FILE = Src/Main.c

WEBCC = emcc
WEBFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file Resources -O3 -o index.html



game: $(FILE)
	$(CC) $(FILE) $(CFLAGS) $(uINCLUDE)

game.exe: $(FILE)
	gcc Src/Main.c -Wall -O3 -std=c99 -lmingw32 -lSDL2main -lSDL2 -o Game -mwindows

game.html: $(FILE)
	$(WEBCC) $(FILE) $(WEBFLAGS)


