#include "pacman.h"
#include "gameboard.h"
#include "t6963c.h"
#include "ghosts.h"
#include "buffer.h"

/**
 * Returns true if Pacman can move to the position
 * or false otherwise
 */
unsigned char CanMove(unsigned char x, unsigned y) {
   unsigned char possibleMove;
   unsigned char charAtPosition = T6963C_readFrom(x, y);
   switch(charAtPosition) {
      case OBSTACLE_A:
      case OBSTACLE_B:
      case OBSTACLE_C:
      case OBSTACLE_D:
      case OBSTACLE_E:
      case OBSTACLE_F:
      case OBSTACLE_G:
      case OBSTACLE_H:
	 possibleMove = 0;
	 break;
      default:
	 possibleMove = 1;
	 break;
   }
   return possibleMove;
}

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
   if (CanMove(new_x, new_y) == 1) {
      /* Erase last position pacman */
      BUFFER_in(pacman->position.x);
      BUFFER_in(pacman->position.y);
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
      case GHOST1:
      case GHOST2:
      case GHOST3:
	 if (pacman->status == BERZERK) {
	    /* Dead ghost */
	    Ghost_Dies(charAtPosition);
	    pacman->points += GHOST_POINTS;
	 } else {
	    pacman->status = DEAD;
	 }	 
	 break;
   }
}

/**
 * Returns the character to be written to the display
 * depending on the context
 */
unsigned char GetBodyCharacter(Pacman *pacman) {
   unsigned char body;
   if (pacman->status == POWERUP) {
      body = PACMAN_POWERUP;
   } else if (pacman->status == DEAD) {
      body = PACMAN_DEAD;
   } else {
      /* Alternate between body 1 and body 2 */
      if (pacman->lastHeadCharacter == PACMAN_BODY1) {
	 pacman->lastHeadCharacter = body = PACMAN_BODY2;
      } else {
	 pacman->lastHeadCharacter = body = PACMAN_BODY1;
      }
   }
   return body;
}

/**
 * Shows the pacman head at the current position
 */
void Pacman_showHead(Pacman *pacman) {
   unsigned char old_x = BUFFER_out(), old_y = BUFFER_out();
   T6963C_writeAt(old_x, old_y, EMPTY);
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
