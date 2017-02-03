#include <mcs51reg.h>
#include "stdio-t6963c.h"
#include "test.h"
#include "buffer.h"
#include "pacman.h"
#include "keyboard.h"
#include "gameboard.h"
#include "ghosts.h"
#include "cherry.h"
#include "t6963c.h"

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

unsigned char game_end = 0, game_paused = 0;

#define HUD_LIFE_X 0
#define HUD_LIFE_Y 0

void displayLives(Pacman *pacman) {
   unsigned int i;
   for(i = 0; i < pacman->livesLeft; i++) {
      T6963C_writeAt(HUD_LIFE_X + i, HUD_LIFE_Y, HEART);   
   }   
}

void displayPoints(unsigned int points) {
   /* TODO */
}

void displayHUD(Pacman *pacman) {
   displayLives(pacman);
   displayPoints(pacman->points);
}
	    
/**
 * Initialize the game board
 */	    
void initGameBoard(Pacman* pacman) {
        /* Clear the board */
	GMB_clear(SCREEN_LIMIT_X0, SCREEN_LIMIT_Y0, SCREEN_LIMIT_X1, SCREEN_LIMIT_Y1);
	GMB_draw(PACMAN_LIMIT_X0, PACMAN_LIMIT_Y0, PACMAN_LIMIT_X1, PACMAN_LIMIT_Y1);
	/* Draw the level */
	GMB_drawLevel();
	/* Draw HUD */
	displayHUD(pacman);
}   
	
/**
 * Initialize the Pacman
 */
void initPacman(Pacman* pacman) {
   pacman->direction = MOVES_RIGHT;
   pacman->position.x = 10;
   pacman->position.y = 10;
   pacman->status = ALIVE;
}

/**
 * Initialize the ghosts
 */
void initGhosts(Ghost* ghosts) {
      ghosts[0].direction = MOVES_LEFT;
      ghosts[0].position.x = GHOST1_SPAWN_X;
      ghosts[0].position.y = GHOST1_SPAWN_Y;
      ghosts[0].status = GHOST_NORMAL;
      ghosts[1].direction = MOVES_UP;
      ghosts[1].position.x = GHOST2_SPAWN_X;
      ghosts[1].position.y = GHOST2_SPAWN_Y;
      ghosts[1].status = GHOST_NORMAL;  
      ghosts[2].direction = MOVES_RIGHT;
      ghosts[2].position.x = GHOST3_SPAWN_X;
      ghosts[2].position.y = GHOST3_SPAWN_Y;
      ghosts[2].status = GHOST_NORMAL;
      /* Place the ghosts */
      Ghost_PlaceAll(ghosts);   
}

/**
 * Handles the player death
 */
void handlePlayerDeath(Pacman *pacman) {
   GMB_display(15, 6, "You died!");
   pacman->livesLeft--;
   if (pacman->livesLeft > 0) {
      game_paused = 1;
      GMB_display(1, 8, "Press any key to continue");
   } else {
      game_end = 1;
   }
}

void play() {
	unsigned char iterate_ghost = 1;
	unsigned char *keyboard = (unsigned char __xdata *) 0x3000;
	Arrow arrow;
	Status status;

	/* Initialize the game board */
	initGameBoard(&pacman);
	/* Initialize the pacman */
	initPacman(&pacman);
        /* Initialize the ghosts */
	initGhosts(ghosts);
   
	Cherry_Place();
   
	do {
		arrow = KEYBOARD_readArrows(keyboard);
	        if (game_paused == 0) {
		  status = Pacman_iterate(&pacman, arrow);
		  if (status == DEAD) {
		     /* Player died */
		     handlePlayerDeath(&pacman);
		     
		     
		  }
		  /* Only iterate the ghost once for two iteration
		     Otherwise the ghosts are moving too fast */
		  if (iterate_ghost) {
		     Ghost_Iterate(ghosts);  
		  }
		  iterate_ghost ^= 1;
		} else {
		   /* Game is paused */
		   if (arrow != ARROW_NEUTRAL) {
		      /* Player touched a key */
		      /* Initialize the game board */
		      initGameBoard(&pacman);
		      /* Initialize the pacman */
		      initPacman(&pacman);
		      /* Initialize the ghosts */
		      initGhosts(ghosts); 
		      game_paused = 0;
		   }
		}
		pause(15000);
	} while (game_end == 0);
	GMB_display(3, 7, " Game Over!");
}

void EventGhostDies(unsigned char ghostCharacter) {
   Ghost_Dies(ghosts, ghostCharacter);
}

void EventGhostsWeak(void) {
   Ghost_SetStatus(&ghosts[0], 1);
   Ghost_SetStatus(&ghosts[1], 1);
   Ghost_SetStatus(&ghosts[2], 1);
   /* TODO Set timer and put back ghosts to normal after a while */
}

void main(void) {

	initialize();
	play();

	while(1);
}

#else
#include <stdio.h>
#include "ghosts.h"

Ghost ghosts[GHOSTS_NB];

void EventGhostDies(unsigned char ghostCharacter) {
   return;
}

void EventGhostsWeak(void) {
   return;
}

void main(void) {
	int testsInError = 0;
	STDIO_initialize();

	//testsInError += testBuffer();
	//testsInError += testKeyboard();
	//testsInError += testGameboard();
	 testsInError += testPacman();

	printf("%d tests en erreur", testsInError);

	while(1);
}
#endif