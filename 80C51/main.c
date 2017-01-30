#include <mcs51reg.h>
#include "stdio-t6963c.h"
#include "test.h"
#include "buffer.h"
#include "pacman.h"
#include "keyboard.h"
#include "gameboard.h"
#include "ghosts.h"
#include "cherry.h"

#ifndef TEST

void pause(unsigned int t) {
	unsigned int n;
	for (n=0; n < t; n++);
}

void initialize() {
	STDIO_initialize();
	GMB_initialize();
}

	Pacman pacman = {MOVES_RIGHT, {10, 10}, ALIVE, 3, 0, BODY2};
	Ghost ghosts[GHOSTS_NB] = { 
	    {0, MOVES_LEFT, {GHOST1_SPAWN_X, GHOST1_SPAWN_Y}, GHOST_NORMAL}, 
	    {1, MOVES_UP, {GHOST2_SPAWN_X, GHOST2_SPAWN_Y}, GHOST_NORMAL}, 
	    {2, MOVES_RIGHT, {GHOST3_SPAWN_X, GHOST3_SPAWN_Y}, GHOST_NORMAL} };

void play() {
	 unsigned char iterate_ghost = 1;
	unsigned char *keyboard = (unsigned char __xdata *) 0x3000;
	Arrow arrow;
	Status status;

	GMB_draw(PACMAN_LIMIT_X0, PACMAN_LIMIT_Y0, PACMAN_LIMIT_X1, PACMAN_LIMIT_Y1);
	/* Draw the level */
	GMB_drawLevel();
	/* Place the ghosts */
	Ghost_PlaceAll(ghosts);
	/* Place powerups */
   
   
   
	Cherry_Place();
   
	do {
		arrow = KEYBOARD_readArrows(keyboard);
	        status = Pacman_iterate(&pacman, arrow);
		if (status == DEAD) {
		   /* Restart */		
		   GMB_display(6, 5, "You died!");
		   pacman.livesLeft--;
		   
		} else {
		   if (iterate_ghost) {
		     Ghost_Iterate(ghosts);  
		   }
		   iterate_ghost ^= 1;
		}
		pause(15000);
	} while (pacman.livesLeft > 0);
	GMB_display(3, 7, " Game Over!");
}

void EventGhostDies(unsigned char ghostCharacter) {
   Ghost_Dies(ghosts, ghostCharacter);
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
	testsInError += testKeyboard();
	testsInError += testGameboard();
   

	printf("%d tests en erreur", testsInError);

	while(1);
}
#endif