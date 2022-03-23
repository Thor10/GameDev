#include "../Includes/Log.h"
#include "../Includes/Engine.h"
#include "../Includes/Vector.h"

#include "../Structs/Image.h"
#include "../Structs/Entity.h"
#include "../Structs/Draw.h"
#include "../Structs/File.h"

# ifdef __EMSCRIPTEN__ 
    #include <emscripten.h>
    #include <emscripten/html5.h>
# endif

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>


