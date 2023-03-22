
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* tolower */
#include <ncurses.h>

#include "defines.h"

typedef struct {
    uint16_t w;    /* Minesweeper width */
    uint16_t h;    /* Minesweeper height */
    uint8_t* grid; /* Pointer to the minesweeper grid */
} ms_t;

/* parse_resolution: parses a resolution string with format "WIDTHxHEIGHT" using
 * atoi. Saves integers in dst_w and dst_h */
void parse_resolution(uint16_t* dst_w, uint16_t* dst_h, char* src) {
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

int main(int argc, char** argv) {
    ms_t ms = (ms_t){
        DEFAULT_W,
        DEFAULT_H,
        NULL,
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
            if (ms.w == 0 || ms.h == 0) {
                fprintf(stderr, "Invalid format for \"%s\"\n", argv[i - 1]);
                arg_error = true;
                break;
            }
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
                "    %s --resolution WxH  - Launch with specified resolution "
                "(width, height)\n"
                "    %s -r WxH            - Same as --resolution\n",
                argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    initscr();            /* Init ncurses */
    raw();                /* Scan input without pressing enter */
    noecho();             /* Don't print when typing */
    keypad(stdscr, TRUE); /* Enable keypad (arrow keys) */

    /* Char the user is pressing */
    int c = 0;
    do {
        /* TODO: Do stuff */
        mvprintw(0, 0, "Resolution: %dx%d | Char: %c", ms.w, ms.h, c);

        /* Wait for user input */
        c = tolower(getch());

        /* Parse input. 'q' quits and there is vim-like navigation */
        switch (c) {
            case 'k':
            case KEY_UARROW:
                /* TODO: Directions */
                break;
            case 'j':
            case KEY_DARROW:
                /* TODO: Directions */
                break;
            case 'h':
            case KEY_LARROW:
                /* TODO: Directions */
                break;
            case 'l':
            case KEY_RARROW:
                /* TODO: Directions */
                break;
            case KEY_CTRLC:
                c = 'q';
                break;
            case 'q':
            default:
                break;
        }
    } while (c != 'q');

    endwin();
    return 0;
}

