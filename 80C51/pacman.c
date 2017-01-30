#include "pacman.h"
#include "gameboard.h"
#include "t6963c.h"
#include "ghosts.h"
#include "buffer.h"

/* Used to know if the pacman must be erased this iteration */
unsigned char g_erase_pacman = 0;

/**
 * Modifie les coordonnées du pacman selon sa direction.
 * @param pacman La description du pacman.
 */
void Pacman_move(Pacman *pacman) {
   unsigned char new_x = pacman->position.x, new_y = pacman->position.y;
   switch(pacman->direction) {
      case MOVES_DOWN:
	 new_y++;
	 break;
      case MOVES_UP:
	 new_y--;
	 break;      
      case MOVES_LEFT:
	 new_x--;
	 break;
      case MOVES_RIGHT:
	 new_x++;
	 break;       
   }
   /* Only move if it is possible */
   if (GMB_MovePossible(new_x, new_y) == 1) {
      /* Save pacman last position */
      BUFFER_in(pacman->position.x);
      BUFFER_in(pacman->position.y);
      g_erase_pacman = 1;
      pacman->position.x = new_x;
      pacman->position.y = new_y;
   }
}

/**
 * Décide de varier la direction du pacman selon la direction indiquée.
 * @param pacman La description du pacman.
 * @param arrow La direction désirée.
 */
void Pacman_turn(Pacman *pacman, Arrow arrow) {
   if (arrow == ARROW_UP) {
      /* Move up */
      pacman->direction = MOVES_UP;
   }
   if (arrow == ARROW_DOWN) {
      /* Move down */
      pacman->direction = MOVES_DOWN;
   }   
   if (arrow == ARROW_LEFT) {
      /* Move left */
      pacman->direction = MOVES_LEFT;
   }
   if (arrow == ARROW_RIGHT) {
      /* Move right */
      pacman->direction = MOVES_RIGHT;
   }   
}

/**
 * Decide if the pacman live or dies
 * @param pacman La description du pacman
 */
void Pacman_liveOrDie(Pacman *pacman) {
   unsigned char charAtPosition = T6963C_readFrom(pacman->position.x, pacman->position.y);
   switch(charAtPosition) {
      case COIN_SMALL:
	 pacman->points += COIN_SMALL_POINTS;
	 break;
      case COIN_LARGE:
	 pacman->points += COIN_LARGE_POINTS;
	 pacman->status = BERZERK;
	 break;
      case CHERRY:
	 pacman->points += CHERRY_POINTS;
	 break;
      case GHOST1_WEAK:
      case GHOST2_WEAK:
      case GHOST3_WEAK:
	 /* Dead ghost */
	 EventGhostDies(charAtPosition);
	 pacman->points += GHOST_POINTS; 
      case GHOST1_NORMAL:
      case GHOST2_NORMAL:
      case GHOST3_NORMAL:
	 pacman->status = DEAD;
	 break;
   }
}

/**
 * Returns the character to be written to the display
 * depending on the context
 */
unsigned char GetBodyCharacter(Pacman *pacman) {
   unsigned char body;
   if (pacman->status == DEAD) {
      body = PACMAN_DEAD;
   } else {
      /* Alternate between body 1 and body 2 */
      body = PACMAN_UP_BODY1 + pacman->direction * 2 + pacman->lastBody;
      /* Toggle body type */
      pacman->lastBody ^= 1;
   }
   return body;
}

/**
 * Shows the pacman head at the current position
 */
void Pacman_showHead(Pacman *pacman) {
   unsigned char old_x, old_y;
   if (g_erase_pacman == 1) {
      old_x = BUFFER_out();
      old_y = BUFFER_out();
      T6963C_writeAt(old_x, old_y, EMPTY);
      g_erase_pacman = 0;
   }
   T6963C_writeAt(pacman->position.x, pacman->position.y, GetBodyCharacter(pacman));
}

/**
 * Effectue une itération dans la vie du serpent.
 * @param snake La définition du serpent.
 * @return L'état du serpent après l'itération.
 */
Status Pacman_iterate(Pacman *pacman, Arrow arrow) {
	Pacman_turn(pacman, arrow);
	Pacman_move(pacman);
	Pacman_liveOrDie(pacman);
	Pacman_showHead(pacman);
	return pacman->status;
}

#ifdef TEST



int testPacman() {
   int testsInError = 0;
   
   
   
   return testsInError;
}

#endif
