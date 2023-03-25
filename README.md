## Minesweeper
**CLI minesweeper made in C using ncurses.**

### Description
Simple CLI minesweeper with custom resolution (game size), difficulty and
vim-like movement. Currently no color support but feel free to make a Pull
Request.

### Building
```console
$ git clone https://github.com/8dcc/minesweeper
$ cd minesweeper
$ make
...
```

### Usage
#### Parameters:
```console
$ ./minesweeper.out --help
Usage:
    ./minesweeper.out                   - Launch with default resolution
    ./minesweeper.out --help            - Show this help
    ./minesweeper.out -h                - Same as --help
    ./minesweeper.out --keys            - Show the controls
    ./minesweeper.out -k                - Same as --keys
    ./minesweeper.out --resolution WxH  - Launch with specified resolution (width, height)
    ./minesweeper.out -r WxH            - Same as --resolution
    ./minesweeper.out --difficulty N    - Use specified difficulty from 1 to 100. Default: 40
    ./minesweeper.out -d N              - Same as --difficulty
```

#### Keys:
```console
$ ./minesweeper.out --keys
Controls:
    <arrows> - Move in the grid
        hjkl - Move in the grid (vim-like)
     <space> - Reveal tile
    <LMouse> - Reveal clicked bomb
           f - Flag bomb
    <RMouse> - Flag clicked bomb
           r - Reveal all tiles and end game
           q - Quit the game
```

#### Characters:
```
+--------------------------------------------------+
|..................................................|
|..................................................|
|..................................................|
|.......................211........................|
|.......................1 1........................|
|......................31 1........................|
|.....................21  1123.....................|
|..................2221      12....................|
|..................1          111..................|
|..................1211         1..................|
|.....................1  111   12..................|
|..................2211  1F1 113...................|
|..................1   123.1 1.....................|
|..................2   1@..1 11212.................|
|..................11111...2     2.................|
|..........................2     2.................|
|.........................31  1111.................|
|.........................2   1....................|
|.........................3   2....................|
|.........................2   1....................|
+--------------------------------------------------+

 You lost. Press any key to restart.

```

- ` `: Empty tile with no bombs adjacent.
- `.`: Unknown tile that can be revealed using `<space>` or flagged using `f`.
- `F`: Flagged tile that can be unflagged with `f`.
- `@`: You just revealed a tile with a bomb. You lost.
- `<number>`: Empty tile with *N* adjacent bombs.

