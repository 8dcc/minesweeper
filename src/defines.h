
#ifndef _DEFINES_H
#define _DEFINES_H 1

#define DEFAULT_W 50
#define DEFAULT_H 20
#define MIN_W     10
#define MIN_H     10

#define DEFAULT_BOMB_PERCENT 10 /* % */

enum cell_flags {
    FLAG_NONE     = 0x0, /* 0000 */
    FLAG_CLEARED  = 0x1, /* 0001 */
    FLAG_FLAGGED  = 0x2, /* 0010 */
    FLAG_CLEARING = 0x4, /* 0100 */
};

#define UNKN_CH '.'
#define BACK_CH ' '
#define BOMB_CH '@'

/* Special keys */
#define KEY_UARROW 259
#define KEY_DARROW 258
#define KEY_LARROW 260
#define KEY_RARROW 261
#define KEY_CTRLC  3

#endif /* _DEFINES_H */

