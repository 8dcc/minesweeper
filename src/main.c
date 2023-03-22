
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  /* time */
#include <ctype.h> /* tolower */
#include <ncurses.h>

#include "defines.h"

typedef struct {
    uint16_t w;   /* Minesweeper width */
    uint16_t h;   /* Minesweeper height */
    char* grid;   /* Pointer to the minesweeper grid */
    bool playing; /* The user revealed the first cell */
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

/* redraw_grid: redraws the grid based on the ms.grid array */
static void redraw_grid(ms_t* ms) {
    draw_border(ms);

    for (int y = 0; y < ms->h; y++)
        for (int x = 0; x < ms->w; x++)
            mvaddch(y + 1, x + 1, ms->grid[y * ms->w + x]);
}

/* generate_grid: generate a random bomb grid with an empty space from the first
 * user selection */
static void generate_grid(ms_t* ms, point_t start, int total_bombs) {
    for (int bombs = 0; bombs < total_bombs; bombs++) {
        int bomb_y = rand() % ms->h;
        int bomb_x = rand() % ms->w;
        /* printf("%dx%d\n", bomb_y, bomb_x); */

        /* Leave an empty zone arround cursor */
        if (bomb_y > start.y - ((MIN_H - 4) / 2) &&
            bomb_y < start.y + ((MIN_H - 4) / 2) &&
            bomb_x > start.x - ((MIN_W - 4) / 2) &&
            bomb_x < start.x + ((MIN_W - 4) / 2)) {
            bombs--;
            continue;
        }

        ms->grid[bomb_y * ms->w + bomb_x] = BOMB_CH;
    }
}

int main(int argc, char** argv) {
    /* Main minesweeper struct */
    ms_t ms = (ms_t){
        DEFAULT_W,
        DEFAULT_H,
        NULL,
        false,
    };

    /* Parse arguments before ncurses */
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
        } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keys")) {
            printf("Controls:\n"
                   "  <arrows> - Move in the grid\n"
                   "      hjkl - Move in the grid (vim-like)\n"
                   "         f - Flag bomb\n"
                   "   <space> - Reveal cell\n");
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
                "    %s -r WxH            - Same as --resolution\n",
                argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    initscr();            /* Init ncurses */
    raw();                /* Scan input without pressing enter */
    noecho();             /* Don't print when typing */
    keypad(stdscr, TRUE); /* Enable keypad (arrow keys) */

    srand(time(NULL)); /* Init random seed */

    /* Allocate and initialize grid */
    ms.grid = malloc(ms.w * ms.h);
    for (int y = 0; y < ms.h; y++)
        for (int x = 0; x < ms.w; x++)
            ms.grid[y * ms.w + x] = BACK_CH;

    redraw_grid(&ms);

    /* User cursor in the grid, not the screen. Start at first cell */
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
                /*TODO*/
                break;
            case ' ':
                if (!ms.playing) {
                    generate_grid(&ms, cursor,
                                  ms.w * ms.h * DEFAULT_BOMB_PERCENT / 100);
                    ms.playing = true;
                    break;
                }

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

