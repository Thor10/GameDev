## About the Game:
This is a game I made using my engine in which I first modified the engine and 
added particles and then made a small simple game. 

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
Arrow keys, if you hit the walls, the controls get inverted

## 3rd Party Assets Used in the Game:
- [m5x7](https://managore.itch.io/m5x7)
