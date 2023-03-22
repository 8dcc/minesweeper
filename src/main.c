
#include <stdio.h>
#include <ctype.h> /* tolower */
#include <ncurses.h>

#include "defines.h"

int main(int argc, char** argv) {
    initscr();            /* Init ncurses */
    raw();                /* Scan input without pressing enter */
    noecho();             /* Don't print when typing */
    keypad(stdscr, TRUE); /* Enable keypad (arrow keys) */

    /* Char the user is pressing */
    int c = 0;
    do {
        /* TODO: Do stuff */
        mvprintw(0, 0, "Char: %c", c);

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

