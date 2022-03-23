## About the Game:
This is a game made for the first ever N8Jam. It was made using SDL2 & C99. I
got it to work for the web and MacOS, Windows 10, there will be a web build,
mac build and maybe windows build. I only give instructions for mac &
emscripten since getting a windows build is complicated since you need to
get the DLLs for SDL as well but other than that just run ``make game.exe``
on Windows. Never was able to submit it though as it is also very buggy.

## Downloading the Game:
Clone the repo, and then change directory to this folder or just get it from https://devhedronyt.itch.io

## Getting the Requirements:

Download SDL2, from the website or by using homebrew or something. Don't forget
to have a compiler as well. I would suggest using Emscripten to compile as that
might work and let you play the game in web. Also have make installed. To make
it easier to run in the web, install Python as well.

## Running the Game:

For an executable (mac)
```
make game 
./Game
```
For a web build
```
make game.html
if you have Python installed:
    python -m http.server 910 or python3 -m http.server 910
    open your browser, and go to localhost:910
otherwise:
    emrun index.html
```

## Playing the Game:
WASD to move although I have found the arrow keys to work better

## 3rd Party Assets Used in the Game:
- None
