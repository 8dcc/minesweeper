
#ifndef _DEFINES_H
#define _DEFINES_H 1

#define DEFAULT_W 50
#define DEFAULT_H 20
#define MIN_W     10
#define MIN_H     10

#define DEFAULT_DIFFICULTY 40 /* 1-100% */
#define MIN_BOMBS 5
#define MAX_BOMBS 70

/* For cell_t.flags */
enum cell_flags {
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
#define KEY_UARROW 259
#define KEY_DARROW 258
#define KEY_LARROW 260
#define KEY_RARROW 261
#define KEY_CTRLC  3

#endif /* _DEFINES_H */

