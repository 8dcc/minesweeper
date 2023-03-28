
#ifndef _DEFINES_H
#define _DEFINES_H 1

/*
 * This line adds the "reveal surrounding" feature. With this feature, adjacent
 * tiles will get revealed if the user is trying to reveal:
 *   - An already revealed tile
 *   - With adjacent bombs
 *   - With all bombs flagged
 * Comment this line if you don't want this feature.
 */
#define REVEAL_SURROUNDING

/*
 * If you compile the program with USE_COLOR and your terminal supports it, it
 * will render the tiles with color.
 */
#define USE_COLOR

enum color_ids {
    COL_NORM = 0, /* White */
    COL_1    = 1,
    COL_2    = 2,
    COL_3    = 3,
    COL_4    = 4,
    COL_5    = 5,
    COL_6    = 6,
    COL_7    = 7,
    COL_8    = 8,
    COL_9    = 9,
    COL_FLAG = 10, /* Red */
    COL_BOMB = 11, /* Red */
    COL_UNK  = 12, /* Gray */
};

#define SET_COL(col)                 \
    {                                \
        if (use_color) {             \
            attron(COLOR_PAIR(col)); \
        }                            \
    }

#define RESET_COL(col)                \
    {                                 \
        if (use_color) {              \
            attroff(COLOR_PAIR(col)); \
        }                             \
    }

#define BOLD_ON()           \
    {                       \
        if (use_color) {    \
            attron(A_BOLD); \
        }                   \
    }

#define BOLD_OFF()           \
    {                        \
        if (use_color) {     \
            attroff(A_BOLD); \
        }                    \
    }

#define DEFAULT_W 50
#define DEFAULT_H 20
#define MIN_W     10
#define MIN_H     10

/* Bombs won't spawn in a 3x3 grid from the starting reveal */
#define BOMB_MARGIN 3

#define DEFAULT_DIFFICULTY 30 /* 1-100% */
#define MIN_BOMBS          5
#define MAX_BOMBS          60

/* For tile_t.flags */
enum tile_flags {
    FLAG_NONE    = 0x0, /* 0000 */
    FLAG_CLEARED = 0x1, /* 0001 */
    FLAG_FLAGGED = 0x2, /* 0010 */
};

/* For ms_t.playing */
enum playing_flags {
    PLAYING_FALSE = 0x0, /* Just started a game. Grid empty. */
    PLAYING_TRUE  = 0x1, /* Playing a game. Grid filled. */
    PLAYING_CLEAR = 0x2, /* Just initialized the empty grid. Fill with bombs */
};

#define UNKN_CH '.'
#define BACK_CH ' '
#define BOMB_CH '@'
#define FLAG_CH 'F'

/* Special keys */
#define KEY_CTRLC 3

#endif /* _DEFINES_H */

