#ifndef ___GAMEBOARD_H
#define ___GAMEBOARD_H

#ifdef TEST
#define CHAR_BASE 0x00
int testGameboard();
#else
#define CHAR_BASE 0x80
#endif


#define EMPTY 0x00

#define PACMAN_UP_BODY1 (0x11 + CHAR_BASE)
#define PACMAN_UP_BODY2 (0x12 + CHAR_BASE)
#define PACMAN_DOWN_BODY1 (0x13 + CHAR_BASE)
#define PACMAN_DOWN_BODY2 (0x14 + CHAR_BASE)
#define PACMAN_LEFT_BODY1 (0x15 + CHAR_BASE)
#define PACMAN_LEFT_BODY2 (0x16 + CHAR_BASE)
#define PACMAN_RIGHT_BODY1 (0x17 + CHAR_BASE)
#define PACMAN_RIGHT_BODY2 (0x18 + CHAR_BASE)
#define PACMAN_DEAD (0x19 + CHAR_BASE)

#define OBSTACLE_LEFT_DOWN  (0x1A + CHAR_BASE)
#define OBSTACLE_LEFT_UP    (0x1B + CHAR_BASE)
#define OBSTACLE_RIGHT_DOWN (0x1C + CHAR_BASE)
#define OBSTACLE_RIGHT_UP   (0x1D + CHAR_BASE)
#define OBSTACLE_VERTICAL   (0x1E + CHAR_BASE)
#define OBSTACLE_HORIZONTAL (0x1F + CHAR_BASE)
#define OBSTACLE_HORIZONTAL_UP (0x2A + CHAR_BASE)
#define OBSTACLE_HORIZONTAL_DOWN (0x2B + CHAR_BASE)
#define OBSTACLE_VERTICAL_LEFT   (0x2C + CHAR_BASE)
#define OBSTACLE_VERTICAL_RIGHT (0x2E + CHAR_BASE)


#define COIN_SMALL (0x20 + CHAR_BASE)
#define COIN_LARGE (0x21 + CHAR_BASE)
#define CHERRY (0x22 + CHAR_BASE)

#define GHOST1_NORMAL (0x23 + CHAR_BASE)
#define GHOST1_WEAK (0x24 + CHAR_BASE)
#define GHOST2_NORMAL (0x25 + CHAR_BASE)
#define GHOST2_WEAK (0x26 + CHAR_BASE)
#define GHOST3_NORMAL (0x27 + CHAR_BASE)
#define GHOST3_WEAK (0x28 + CHAR_BASE)

#define COIN_SMALL_POINTS 1
#define COIN_LARGE_POINTS 5
#define CHERRY_POINTS 10
#define GHOST_POINTS 200

void GMB_initialize();
void GMB_draw(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void GMB_clear(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void GMB_display(unsigned char x0, unsigned char y0, char *text);
void GMB_drawLevel(void);
unsigned char GMB_MovePossible(unsigned char x, unsigned y);

#endif
