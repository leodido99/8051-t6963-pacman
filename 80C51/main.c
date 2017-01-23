#include <mcs51reg.h>
#include "stdio-t6963c.h"
#include "test.h"
#include "buffer.h"
#include "pacman.h"
#include "keyboard.h"
#include "gameboard.h"

// Snake-0

#ifndef TEST

void pause(unsigned int t) {
	unsigned int n;
	for (n=0; n < t; n++);
}

void initialize() {
	STDIO_initialize();
	GMB_initialize();
}

void play() {
	Pacman pacman = {MOVES_RIGHT, {10, 10}, ALIVE, 3, 0, PACMAN_BODY2};
	unsigned char *keyboard = (unsigned char __xdata *) 0x3000;
	Arrow arrow;
	Status status;

	GMB_draw(SNAKE_LIMIT_X0, SNAKE_LIMIT_Y0, SNAKE_LIMIT_X1, SNAKE_LIMIT_Y1);
	
	/* Place powerups */
	
	do {
		arrow = KEYBOARD_readArrows(keyboard);
	        status = Pacman_iterate(&pacman, arrow);
		if (status == DEAD) {
			/* Restart */
			pacman.livesLeft--;
		}		
		pause(20000);
	} while (pacman.livesLeft > 0);
	GMB_display(3, 7, " Game Over!");
}

void main(void) {

	initialize();
	play();

	while(1);
}

#else
#include <stdio.h>
void main(void) {
	int testsInError = 0;
	STDIO_initialize();

	testsInError += testBuffer();
	testsInError += testSnake();
	testsInError += testKeyboard();
	testsInError += testFruit();
	testsInError += testGameboard();

	printf("%d tests en erreur", testsInError);

	while(1);
}
#endif