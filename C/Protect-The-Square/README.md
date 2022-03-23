## About the Game:
This is the base code for almost all of my SDL projects. It was made using SDL2
& C99.  I got it to work for the web and MacOS, but I did not make a Windows
build. The goal of the game is to avoid the yellow squares at all costs and get
the highest score you can.

## Downloading the Game:
Clone the repo, and then change directory to this folder

## Getting the Requirements:

Download SDL2, from the website or by using homebrew or something. Don't forget
to have a compiler as well. I would suggest using Emscripten to compile as that
might work and let you play the game in web. Also have make installed. To make
it easier to run in the web, install Python as well. [Game
Page](https://devhedronyt.itch.io/protect-the-square)

## Running the Game:

For an executable
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
- Up Arrow -> Move Up
- Down Arrow -> Move Down

## 3rd Party Assets Used in the Game:
- [m5x7 Font](https://managore.itch.io/m5x7)
