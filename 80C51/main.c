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
#include <stdio.h>
#include <string.h>

#ifndef TEST

void pause(unsigned int t) {
	unsigned int n;
	for (n=0; n < t; n++);
}

void initialize() {
	STDIO_initialize();
	GMB_initialize();
}


Pacman pacman = {MOVES_NEUTRAL, {0, 0}, ALIVE, 3, 0, BODY2};

Ghost ghosts[GHOSTS_NB] = { 
   {0, MOVES_DOWN, {GHOST1_SPAWN_X, GHOST1_SPAWN_Y}, GHOST_NORMAL, COIN_SMALL}, 
   {1, MOVES_UP, {GHOST2_SPAWN_X, GHOST2_SPAWN_Y}, GHOST_NORMAL, COIN_SMALL}, 
   {2, MOVES_RIGHT, {GHOST3_SPAWN_X, GHOST3_SPAWN_Y}, GHOST_NORMAL, COIN_SMALL} };

unsigned char game_end = 0, game_paused = 0;
unsigned int weak_ghost_iter = 0;
	    
#define HUD_LIFE_X 0
#define HUD_LIFE_Y 0
	    
/**
 * Set the status of all the ghosts
 */
void setGhostStatus(unsigned int weak) {
   Ghost_SetStatus(&ghosts[0], weak);
   Ghost_SetStatus(&ghosts[1], weak);
   Ghost_SetStatus(&ghosts[2], weak);
}

/**
 * Display number of lives left
 */
void displayLives(Pacman *pacman) {
   unsigned int i;
   for(i = 0; i < pacman->livesLeft; i++) {
      T6963C_writeAt(HUD_LIFE_X + i, HUD_LIFE_Y, HEART);   
   }   
}

/**
 * Display the current number of points
 */
void displayPoints(unsigned int points) {
   char tmp[20];
   sprintf(tmp, "%d", points);
   GMB_draw_text(SCREEN_LIMIT_X1  - strlen(tmp),0, tmp);
}

/** 
 * display the Head on Display
 */
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
   /* Create border around game area */
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
   Pacman_Init(pacman);
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
   GMB_display(9, 3, "You died!");
   pacman->livesLeft--;
   if (pacman->livesLeft > 0) {
      game_paused = 1;
      GMB_display(1, 7, "Press any key to continue!");
   } else {
      game_end = 1;
   }
}

unsigned int old_points = 0;

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
   
	do {
		arrow = KEYBOARD_readArrows(keyboard);
	        if (game_paused == 0) {
		  status = Pacman_iterate(&pacman, arrow);
		  if (status == DEAD) {
		     /* Player died */
		     handlePlayerDeath(&pacman);
		  } else {
		     /* Only iterate the ghost once for two iteration
			Otherwise the ghosts are moving too fast */
		     if (iterate_ghost) {
			Ghost_Iterate(ghosts);  
		     }
		     iterate_ghost ^= 1;
		     
		     /* Check if ghosts are weak */
		     if (Ghost_GetStatus(&ghosts[0]) == 1) {
			if (weak_ghost_iter > 0) {
			   weak_ghost_iter--;
			} else {
			   /* We reached the number of weak ghost iterations */
			   setGhostStatus(0);
			}
		     }
		     /* Display Points if needed */
		     if (old_points != pacman.points) {
			displayPoints(pacman.points);  
			old_points = pacman.points;
		     }
		  }
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

/**
 * Event triggered when a ghost dies
 */
void EventGhostDies(unsigned char ghostCharacter) {
   Ghost_Dies(ghosts, ghostCharacter);
}

#define WEAK_GHOST_NB_ITER 500

/**
 * Event triggered when ghosts become weak
 */
void EventGhostsWeak(void) {
   setGhostStatus(1);
   weak_ghost_iter = WEAK_GHOST_NB_ITER;
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

	testsInError += testBuffer();
	testsInError += testKeyboard();
	testsInError += testGameboard();
	testsInError += testPacman();
	testsInError += testGhost();

	printf("%d tests en erreur", testsInError);

	while(1);
}
#endif