CC = gcc
uINCLUDE = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
wINCLUDE = -lSDL2main -lSDL2 -lmingw32 
CFLAGS = -Wall -O2 -std=c99 -o build/unix/Game -mwindows -I engine/include

FILE = src/main.c engine/src/*.c

WEBCC = emcc
WEBFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file res -O2 -I engine/include -o build/web/index.html 

DEBUG = -g -O0 -std=c99 -o build/unix/Game -I engine/include
EXTRAMACFLAGS = -I/Users/mali/homebrew/Cellar/sdl2/2.0.16/include -L/Users/mali/homebrew/lib  

game: $(FILE)
	$(CC) $(FILE) $(CFLAGS) $(uINCLUDE) 

game.exe: $(FILE)
	gcc Src/Main.c -Wall -O2 -std=c89 -lmingw32 -lSDL2main -lSDL2 -o build/win/Game -mwindows 

game.html: $(FILE)
	$(WEBCC) $(FILE) $(WEBFLAGS)

game.mac: $(FILE)
	$(CC) $(FILE) $(EXTRAMACFLAGS) $(CFLAGS) $(uINCLUDE) 

