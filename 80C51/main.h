#ifndef __MAIN_H
#define __MAIN_H

#define PACMAN_LIMIT_X0 0
#define PACMAN_LIMIT_Y0 0
#define PACMAN_LIMIT_X1 29
#define PACMAN_LIMIT_Y1 15

typedef struct {
	unsigned char x;
	unsigned char y;
} Position;

void EventGhostDies(unsigned char ghostCharacter);

#endif
