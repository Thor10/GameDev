CC = gcc
FILE = Src/Main.c
CFLAGS = -Wall -o Game -std=c99 -O3
INCLUDE = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

WEBCC = emcc
WEBFILE = Src/Main.c
WEBFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file Resources -O3 -o index.html

default:
	mingw32-gcc main.c -I"C:\Users\Artur\Desktop\i686-w64-mingw32\include" -L"C:\Users\Artur\Desktop\i686-w64-mingw32\lib" -lSDL2main -lSDL2

Game: $(FILE) $(WEBFILE)
	$(CC) $(FILE) $(CFLAGS) $(INCLUDE)
	$(WEBCC) $(WEBFILE) $(WEBFLAGS) 

Executable: $(FILE)
	$(CC) $(FILE) $(CFLAGS) $(INCLUDE)

Web: $(WEBFILE)
	$(WEBCC) $(WEBFILE) $(WEBFLAGS) 

DeleteAll: 
	rm *.html
	rm *.data
	rm *.wasm
	rm *.js
	rm Game

DeleteExecutable:
	rm Game

DeleteWeb:
	rm *.html
	rm *.data
	rm *.wasm
	rm *.js
	
