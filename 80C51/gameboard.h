#ifndef ___GAMEBOARD_H
#define ___GAMEBOARD_H

#ifdef TEST
#define CHAR_BASE 0x00
int testGameboard();
#else
#define CHAR_BASE 0x80
#endif


#define EMPTY 0x00

#define OBSTACLE_A (0x21 + CHAR_BASE)
#define OBSTACLE_B (0x22 + CHAR_BASE)
#define OBSTACLE_C (0x23 + CHAR_BASE)
#define OBSTACLE_D (0x24 + CHAR_BASE)
#define OBSTACLE_E (0x25 + CHAR_BASE)
#define OBSTACLE_F (0x26 + CHAR_BASE)
#define OBSTACLE_G (0x27 + CHAR_BASE)
#define OBSTACLE_H (0x28 + CHAR_BASE)

#define PACMAN_BODY1 (0x11 + CHAR_BASE)
#define PACMAN_BODY2 (0x11 + CHAR_BASE)
//#define PACMAN_BODY2 (0x12 + CHAR_BASE)
#define PACMAN_POWERUP (0x14 + CHAR_BASE)
#define PACMAN_DEAD (0x15 + CHAR_BASE)

#define COIN_SMALL (0x16 + CHAR_BASE)
#define COIN_LARGE (0x17 + CHAR_BASE)
#define CHERRY (0x18 + CHAR_BASE)

#define GHOST1 (0x19 + CHAR_BASE)
#define GHOST2 (0x1A + CHAR_BASE)
#define GHOST3 (0x1B + CHAR_BASE)

#define COIN_SMALL_POINTS 1
#define COIN_LARGE_POINTS 5
#define CHERRY_POINTS 10
#define GHOST_POINTS 200

void GMB_initialize();
void GMB_draw(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void GMB_clear(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void GMB_display(unsigned char x0, unsigned char y0, char *text);

#endif
