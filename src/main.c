
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  /* time */
#include <ctype.h> /* tolower */
#include <ncurses.h>

#include "defines.h"

#define DIFFIC2BOMBPERCENT(d) ((MAX_BOMBS - MIN_BOMBS) * d / 100 + MIN_BOMBS)

typedef struct {
    char c;        /* Char in that tile, actual item */
    uint8_t flags; /* Tile status (revealed, flaged, etc) */
} tile_t;

typedef struct {
    uint16_t w;         /* Minesweeper width */
    uint16_t h;         /* Minesweeper height */
    tile_t* grid;       /* Pointer to the minesweeper grid */
    uint8_t playing;    /* The user revealed the first tile */
    uint8_t difficulty; /* Percentage of bombs to fill in the grid */
} ms_t;

typedef struct {
    uint16_t y;
    uint16_t x;
} point_t;

/* parse_resolution: parses a resolution string with format "WIDTHxHEIGHT" using
 * atoi. Saves integers in dst_w and dst_h */
static void parse_resolution(uint16_t* dst_w, uint16_t* dst_h, char* src) {
    *dst_w = 0;
    *dst_h = 0;

    char* start = src;
    while (*src != 'x' && *src != '\0')
        src++;

    /* No x, invalid format */
    if (*src != 'x')
        return;

    /* Cut string at 'x', make it point to start of 2nd digit */
    *src++ = '\0';

    *dst_w = atoi(start);
    *dst_h = atoi(src);
}

/* draw_border: draws the grid border for the ms */
static void draw_border(ms_t* ms) {
    /* First line */
    mvaddch(0, 0, '+');
    for (int x = 0; x < ms->w; x++)
        mvaddch(0, x + 1, '-');
    mvaddch(0, ms->w + 1, '+');

    /* Mid lines */
    for (int y = 1; y <= ms->h; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, ms->w + 1, '|');
    }

    /* Last line */
    mvaddch(ms->h + 1, 0, '+');
    for (int x = 0; x < ms->w; x++)
        mvaddch(ms->h + 1, x + 1, '-');
    mvaddch(ms->h + 1, ms->w + 1, '+');
}

/* init_grid: initializes the empty background grid for the ms struct */
static void init_grid(ms_t* ms) {
    for (int y = 0; y < ms->h; y++) {
        for (int x = 0; x < ms->w; x++) {
            ms->grid[y * ms->w + x].c     = BACK_CH;
            ms->grid[y * ms->w + x].flags = FLAG_NONE;
        }
    }
}

/* get_bombs: returns the number of bombs surrounding a specified tile */
static int get_bombs(ms_t* ms, int fy, int fx) {
    int ret = 0;

    /* ###
     * #X#
     * ### */
    for (int y = fy - 1; y <= fy + 1; y++)
        for (int x = fx - 1; x <= fx + 1; x++)
            if (ms->grid[y * ms->w + x].c == BOMB_CH)
                ret++;

    return ret;
}

/* redraw_grid: redraws the grid based on the ms.grid array */
static void redraw_grid(ms_t* ms) {
    draw_border(ms);

    for (int y = 0; y < ms->h; y++) {
        for (int x = 0; x < ms->w; x++) {
            if (ms->grid[y * ms->w + x].flags & FLAG_CLEARED) {
                const int bombs = get_bombs(ms, y, x);
                if (bombs && ms->grid[y * ms->w + x].c != BOMB_CH)
                    /* Number */
                    mvaddch(y + 1, x + 1, bombs + '0');
                else
                    /* Empty or bomb (we lost) */
                    mvaddch(y + 1, x + 1, ms->grid[y * ms->w + x].c);
            } else if (ms->grid[y * ms->w + x].flags & FLAG_FLAGGED) {
                mvaddch(y + 1, x + 1, FLAG_CH);
            } else {
                mvaddch(y + 1, x + 1, UNKN_CH);
            }
        }
    }
}

/* generate_grid: generate a random bomb grid with an empty space from the first
 * user selection */
static void generate_grid(ms_t* ms, point_t start, int bomb_percent) {
    int total_bombs = ms->h * ms->w * bomb_percent / 100;

    /* Actual tiles available for bombs (keep in mind the empty zone around the
     * cursor) */
    const int max_bombs = ms->w * ms->h - (MIN_H - 8) - (MIN_W - 8);
    if (total_bombs > max_bombs) {
#ifdef DEBUG
        fprintf(stderr,
                "generate_grid: Error. Can't generate %d bombs (%d%%) in grid "
                "%dx%d.\n",
                total_bombs, bomb_percent, ms->h, ms->w);
        return;
#endif
        total_bombs = max_bombs;
    }

    for (int bombs = 0; bombs < total_bombs; bombs++) {
        int bomb_y = rand() % ms->h;
        int bomb_x = rand() % ms->w;

        /* Leave an empty zone around cursor */
        if (bomb_y > start.y - ((MIN_H - 4) / 2) &&
            bomb_y < start.y + ((MIN_H - 4) / 2) &&
            bomb_x > start.x - ((MIN_W - 4) / 2) &&
            bomb_x < start.x + ((MIN_W - 4) / 2)) {
            bombs--;
            continue;
        }

        ms->grid[bomb_y * ms->w + bomb_x].c = BOMB_CH;
    }
}

/* print_message: prints the specified message 2 lines bellow ms's grid */
static void print_message(ms_t* ms, const char* str) {
    int y, x;
    getyx(stdscr, y, x);

    mvprintw(ms->h + 3, 1, "%s", str);

    move(y, x);
}

/* clear_line: clears a line in the screen */
static inline void clear_line(int y) {
    int oy, ox;
    getyx(stdscr, oy, ox);

    move(y, 0);
    clrtoeol();

    move(oy, ox);
}

/* reveal_tiles: reveals the needed tiles using recursion, based on y and x */
static void reveal_tiles(ms_t* ms, int fy, int fx) {
    if (ms->grid[fy * ms->w + fx].c == BOMB_CH) {
        print_message(ms, "You lost. Press any key to restart.");
        ms->grid[fy * ms->w + fx].flags |= FLAG_CLEARED;
        ms->playing = PLAYING_FALSE;
        return;
    }

    ms->grid[fy * ms->w + fx].flags |= FLAG_CLEARED;

    if (!get_bombs(ms, fy, fx)) {
        /* No bombs, reveal surrounding tiles
         * ###
         * #X#
         * ### */
        for (int y = (fy > 0) ? fy - 1 : fy; y <= fy + 1 && y < ms->h; y++)
            for (int x = (fx > 0) ? fx - 1 : fx; x <= fx + 1 && x < ms->w; x++)
                /* If we are not revealing that one, reveal */
                if (!(ms->grid[y * ms->w + x].flags & FLAG_CLEARED))
                    reveal_tiles(ms, y, x);
    }
}

int main(int argc, char** argv) {
    /* Main minesweeper struct */
    ms_t ms = (ms_t){
        .w          = DEFAULT_W,
        .h          = DEFAULT_H,
        .grid       = NULL,
        .playing    = PLAYING_FALSE,
        .difficulty = DEFAULT_DIFFICULTY,
    };

    /* Parse arguments before ncurses */
    /* TODO: Move to func */
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--resolution")) {
            if (i == argc - 1) {
                fprintf(stderr, "Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            i++;
            parse_resolution(&ms.w, &ms.h, argv[i]);
            if (ms.w < MIN_W || ms.h < MIN_H) {
                fprintf(stderr,
                        "Invalid resolution format for \"%s\".\n"
                        "Minimum resolution: %dx%d\n",
                        argv[i - 1], MIN_W, MIN_H);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--difficulty")) {
            if (i == argc - 1) {
                fprintf(stderr, "Not enough arguments for \"%s\"\n", argv[i]);
                arg_error = true;
                break;
            }

            ms.difficulty = atoi(argv[++i]);
            if (ms.difficulty < 1 || ms.difficulty > 100) {
                fprintf(stderr,
                        "Invalid difficulty format for \"%s\".\n"
                        "Difficulty range: 1-100\n",
                        argv[i - 1]);
                arg_error = true;
                break;
            }
        } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keys")) {
            printf("Controls:\n"
                   "  <arrows> - Move in the grid\n"
                   "      hjkl - Move in the grid (vim-like)\n"
                   "         f - Flag bomb\n"
                   "   <space> - Reveal tile\n"
                   "         r - Reveal all tiles and end game\n");
            return 0;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            arg_error = true;
            break;
        }
    }

    if (arg_error) {
        fprintf(stderr,
                "Usage:\n"
                "    %s                   - Launch with default resolution\n"
                "    %s --help            - Show this help\n"
                "    %s -h                - Same as --help\n"
                "    %s --keys            - Show the controls\n"
                "    %s -k                - Same as --keys\n"
                "    %s --resolution WxH  - Launch with specified resolution "
                "(width, height)\n"
                "    %s -r WxH            - Same as --resolution\n"
                "    %s --difficulty N    - Use specified difficulty from 1 to "
                "100. Default: 40\n"
                "    %s -d N              - Same as --difficulty\n",
                argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0],
                argv[0], argv[0]);
        return 1;
    }

    initscr();            /* Init ncurses */
    raw();                /* Scan input without pressing enter */
    noecho();             /* Don't print when typing */
    keypad(stdscr, TRUE); /* Enable keypad (arrow keys) */

    srand(time(NULL)); /* Init random seed */

    /* Allocate and initialize grid */
    ms.grid = malloc(ms.w * ms.h * sizeof(tile_t));
    init_grid(&ms);
    ms.playing = PLAYING_CLEAR;

    redraw_grid(&ms);

    /* User cursor in the grid, not the screen. Start at first tile */
    point_t cursor = (point_t){ 0, 0 };

    /* Char the user is pressing */
    int c = 0;
    do {
        /* First, redraw the grid */
        redraw_grid(&ms);

        /* Update the cursor (+margins) */
        move(cursor.y + 1, cursor.x + 1);

        /* Refresh screen */
        refresh();

        /* Wait for user input */
        c = tolower(getch());

        /* Clear the output line */
        clear_line(ms.h + 3);

        /* If it's the first iteration on a new game, clear grid. We will only
         * generate the bombs once we press space the first time */
        if (ms.playing == PLAYING_FALSE) {
            init_grid(&ms);
            ms.playing = PLAYING_CLEAR;
        }

        /* Parse input. 'q' quits and there is vim-like navigation */
        switch (c) {
            case 'k':
            case KEY_UARROW:
                if (cursor.y > 0)
                    cursor.y--;
                break;
            case 'j':
            case KEY_DARROW:
                if (cursor.y < ms.h - 1)
                    cursor.y++;
                break;
            case 'h':
            case KEY_LARROW:
                if (cursor.x > 0)
                    cursor.x--;
                break;
            case 'l':
            case KEY_RARROW:
                if (cursor.x < ms.w - 1)
                    cursor.x++;
                break;
            case 'f':
                /* If we just started playing, but we don't have the bombs */
                if (ms.playing == PLAYING_CLEAR) {
                    print_message(&ms, "Can't flag a tile before revealing!");
                    break;
                }

                /* TODO */

                break;
            case ' ':
                /* Initialize the bombs once we reveal for the first time */
                if (ms.playing == PLAYING_CLEAR) {
                    generate_grid(&ms, cursor,
                                  DIFFIC2BOMBPERCENT(ms.difficulty));
                    ms.playing = PLAYING_TRUE;
                }

                reveal_tiles(&ms, cursor.y, cursor.x);
                break;
            case 'r':
                /* Generate if it's the first time playing */
                if (ms.playing == PLAYING_CLEAR) {
                    generate_grid(&ms, cursor,
                                  DIFFIC2BOMBPERCENT(ms.difficulty));
                    ms.playing = PLAYING_TRUE;
                }

                print_message(&ms, "Revealing all tiles and aborting game. "
                                   "Press any key to continue.");

                for (int y = 0; y < ms.h; y++)
                    for (int x = 0; x < ms.w; x++)
                        ms.grid[y * ms.w + x].flags |= FLAG_CLEARED;

                ms.playing = PLAYING_FALSE;
                break;
            case KEY_CTRLC:
                c = 'q';
                break;
            case 'q':
            default:
                break;
        }
    } while (c != 'q');

    free(ms.grid);
    endwin();
    return 0;
}

