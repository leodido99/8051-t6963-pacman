
#include "ghosts.h"
#include "t6963c.h"
#include "gameboard.h"
#include <stdlib.h>

/**
 * Returns the ghost character from an index and status
 */
unsigned char GetCharacterFromIndex(unsigned char index, GhostStatus status) {
   return GHOST1_NORMAL + index * 2 + status;
}   

/**
 * Return the index of a ghost in the array from its character
 */
unsigned char GetIndexFromCharacter(unsigned char character) {
   unsigned char index = 0;
   switch(character) {
      case GHOST1_NORMAL:
      case GHOST1_WEAK:
	 index = 0;
	 break;
      case GHOST2_NORMAL:
      case GHOST2_WEAK:
	 index = 1;
	 break; 
      case GHOST3_NORMAL:
      case GHOST3_WEAK:
	 index = 2;
	 break;      
   }
   return index;
}
   
/*
 * Place all the ghosts at their starting points
 */
void Ghost_PlaceAll(Ghost *ghosts) {
   unsigned char i;
   for(i = 0; i < GHOSTS_NB; i++) {
      T6963C_writeAt(ghosts[i].position.x, ghosts[i].position.y, GetCharacterFromIndex(i, ghosts[i].status));
   }
}

/**
 * Returns the new position of the ghost depending on its direction
 */
void Ghost_GetNewPosition(Direction direction, unsigned char* new_x, unsigned char* new_y) {
   switch(direction) {
      case MOVES_DOWN:
	 (*new_y)++;
	 break;
      case MOVES_UP:
	 (*new_y)--;
	 break;      
      case MOVES_LEFT:
	 (*new_x)--;
	 break;
      case MOVES_RIGHT:
	 (*new_x)++;
	 break;       
   }   
}

/**
 * Display the ghost
 */
void Ghost_Show(Ghost *ghost) {
   /* Draw ghost */
   T6963C_writeAt(ghost->position.x, ghost->position.y, GetCharacterFromIndex(ghost->ghost_index, ghost->status));
}

void Ghost_SetStatus(Ghost *ghost, unsigned int weak) {
   unsigned int status = GHOST_NORMAL;
   if (weak == 1) {
      status = GHOST_WEAK;
   }   
   ghost->status = status;
}

/**
 * Move the ghost
 */
void Ghost_Move(Ghost *ghost) {
   unsigned char new_x = ghost->position.x, new_y = ghost->position.y;
   Ghost_GetNewPosition(ghost->direction, &new_x, &new_y);
   /* Clear old ghost */
   T6963C_writeAt(ghost->position.x, ghost->position.y, EMPTY);
   ghost->position.x = new_x;
   ghost->position.y = new_y;
}

/**
 * Returns a random direction
 */
Direction GetRandomDirection(void) {
   int randVal;
   randVal = rand() % 4;
   return randVal;
}

/**
 * Returns a direction which a ghost is free to move to
 */
Direction GetFreeDirection(Ghost *ghost) {
   Direction new_direction = ghost->direction;
   unsigned char move_possible, new_x, new_y;
   
   do {
      /* Get the next position with the current direction */
      new_x = ghost->position.x;
      new_y = ghost->position.y;
      Ghost_GetNewPosition(new_direction, &new_x, &new_y);
      move_possible = GMB_MovePossible(new_x, new_y);
      if (move_possible == 0) {
	 /* Move impossible */
	 new_direction++;
	 if (new_direction > MOVES_RIGHT) {
	    new_direction = MOVES_UP;
	 }
      }
   } while(move_possible == 0);
   return new_direction;
}

/**
 * Returns 1 if the character is a ghost, 0 otherwise
 */
unsigned char charIsGhost(unsigned char character) {
   unsigned char isGhost;
   switch(character) {
      case GHOST1_NORMAL:
      case GHOST1_WEAK:
      case GHOST2_NORMAL:
      case GHOST2_WEAK:
      case GHOST3_NORMAL:
      case GHOST3_WEAK:	 
	 isGhost = 1;
	 break;
      default:
	 isGhost = 0;
	 break;
   }
   return isGhost;
}

unsigned char Ghost_CanMove(unsigned char new_x, unsigned char new_y) {
   unsigned char charAtPos = T6963C_readFrom(new_x, new_y);
   unsigned char canMove = 0;
   /* We can move if there is no obstacle or ghost in the new position */
   if (GMB_MovePossible(new_x, new_y) == 1 && charIsGhost(charAtPos) == 0) {
      canMove = 1;
   }
   return canMove;
}

/**
 * Decides if it is time to randomly change direction
 */
unsigned char RandomDirectionChange(void) {
   unsigned char direction_change = 0;
 
 
 
   return direction_change;
}

/**
 * Modifies the direction of the ghost if required
 * Only modified if the ghost cannot move to the next position
 * or if it's randomly time to change.
 */
void Ghost_Turn(Ghost *ghost) {
   unsigned char new_x = ghost->position.x, new_y = ghost->position.y;
   /* Get the next position */
   Ghost_GetNewPosition(ghost->direction, &new_x, &new_y);
   if (GMB_MovePossible(new_x, new_y) == 0 || RandomDirectionChange() == 1) {
      /* We cannot go to the next position, 
         in all cases we have to change it. 
         Or it is time to randomly change direction */
      ghost->direction = GetFreeDirection(ghost);
   }
}

/**
 * Reset the ghost status and position and draw it
 */
void Ghost_Respawn(Ghost *ghost) {
   switch(ghost->ghost_index) {
      case 0:
	 ghost->position.x = GHOST1_SPAWN_X;
	 ghost->position.y = GHOST1_SPAWN_Y;
	 break;
      case 1:
	 ghost->position.x = GHOST2_SPAWN_X;
	 ghost->position.y = GHOST2_SPAWN_Y;
	 break;
      case 2:
	 ghost->position.x = GHOST3_SPAWN_X;
	 ghost->position.y = GHOST3_SPAWN_Y;
	 break;
   }
   ghost->status = GHOST_NORMAL;
}

/*
 * Perform an iteration of the ghosts
 */
void Ghost_Iterate(Ghost *ghosts) {
   unsigned char i;
   for(i = 0; i < GHOSTS_NB; i++) {
      if (ghosts[i].status != GHOST_DEAD) {
	 /* Update ghost direction if needed */
	 Ghost_Turn(&ghosts[i]);
	 /* Move the Ghost */
	 Ghost_Move(&ghosts[i]);
      } else {
	 /* Ghost is dead */
	 Ghost_Respawn(&ghosts[i]);
      }
      /* Show the ghost */
      Ghost_Show(&ghosts[i]);
   }
}

void Ghost_Dies(Ghost *ghosts, unsigned char ghostChar) {
   unsigned char idx = GetIndexFromCharacter(ghostChar);
   if (idx <= (GHOSTS_NB-1)) {
      ghosts[idx].status = GHOST_DEAD;  
   }
}
