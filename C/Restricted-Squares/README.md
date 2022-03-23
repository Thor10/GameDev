## About the Game:
This game was made for the Free Palestine Game Jam. Itch.io Link:
https://devhedronyt.itch.io/restricted-square. It was made using SDL2 & C99. I
got it to work for the web and MacOS, but I could not get it to work for
Windows 10.

## Downloading the Game:
Clone the repo, and then change directory to this folder

## Getting the Requirements:

Download SDL2, from the website or by using homebrew or something. Don't forget
to have a compiler as well. I would suggest using Emscripten to compile as that
might work and let you play the game in web. Also have make installed. To make
it easier to run in the web, install Python as well.

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
Arrow Keys to move
Spacebar to restart game 

## 3rd Party Assets Used in the Game:

- https://managore.itch.io/m5x7
