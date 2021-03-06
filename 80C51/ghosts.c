
#include "ghosts.h"
#include "t6963c.h"
#include "gameboard.h"
#include <stdlib.h>
#include "test.h"

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

/**
 * Returns 1 if the character is a pacman, 0 otherwise
 */
unsigned char charIsPacman(unsigned char character) {
   unsigned char isGhost;
   switch(character) {
      case PACMAN_UP_BODY1:
      case PACMAN_UP_BODY2:
      case PACMAN_DOWN_BODY1:
      case PACMAN_DOWN_BODY2:
      case PACMAN_LEFT_BODY1:
      case PACMAN_LEFT_BODY2:
      case PACMAN_RIGHT_BODY1:
      case PACMAN_RIGHT_BODY2:	 
	 isGhost = 1;
	 break;
      default:
	 isGhost = 0;
	 break;
   }
   return isGhost;
}

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
void Ghost_Show(unsigned char idx, Ghost *ghosts) {
   /* Save whatever is at the ghost position */
   ghosts[idx].prevChar = T6963C_readFrom(ghosts[idx].position.x, ghosts[idx].position.y);
   if (charIsGhost(ghosts[idx].prevChar) == 1) {
      /* Previous char is a ghost */
      ghosts[idx].prevChar = ghosts[GetIndexFromCharacter(ghosts[idx].prevChar)].prevChar;
   } else if (charIsPacman(ghosts[idx].prevChar) == 1) {
      /* Previous char is a pacman*/
      ghosts[idx].prevChar = EMPTY;
   }
   /* Draw ghost */
   T6963C_writeAt(ghosts[idx].position.x, ghosts[idx].position.y, GetCharacterFromIndex(ghosts[idx].ghost_index, ghosts[idx].status));
}

/** 
 * Return the status of the ghost
 */
unsigned int Ghost_GetStatus(Ghost *ghost) {
   unsigned int status = 0;
   if (ghost->status == GHOST_WEAK) {
      status = 1;
   }
   return status;
}

/** 
 * Set the status of the ghost
 */
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
   /* Clear old ghost, write back old char */
   T6963C_writeAt(ghost->position.x, ghost->position.y, ghost->prevChar);
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
   Direction new_direction = GetRandomDirection();
   unsigned char move_possible, new_x, new_y;
   do {
      /* Get the next position with the current direction */
      new_x = ghost->position.x;
      new_y = ghost->position.y;
      Ghost_GetNewPosition(new_direction, &new_x, &new_y);
      move_possible = GMB_MovePossible(new_x, new_y);
      if (move_possible == 0) {
	 /* Move impossible */
	 new_direction = GetRandomDirection();
      }
   } while(move_possible == 0);
   return new_direction;
}

/*
 * Returns 1 if the ghost can move to this position, 0 otherwise
 */
unsigned char Ghost_CanMove(unsigned char new_x, unsigned char new_y) {
   unsigned char charAtPos = T6963C_readFrom(new_x, new_y);
   unsigned char canMove = 0;
   /* We can move if there is no obstacle or ghost in the new position */
   if (GMB_MovePossible(new_x, new_y) == 1 && charIsGhost(charAtPos) == 0) {
      canMove = 1;
   }
   return canMove;
}

/* Counters for the ghost going in the same direction */
unsigned int ghost_same_dir[3] = { 0,0,0 };

/**
 * Modifies the direction of the ghost if required
 * Only modified if the ghost cannot move to the next position
 * or if it's randomly time to change.
 */
void Ghost_Turn(Ghost *ghost) {
   unsigned char new_x = ghost->position.x, new_y = ghost->position.y;
   /* Get the next position */
   Ghost_GetNewPosition(ghost->direction, &new_x, &new_y);
   if (GMB_MovePossible(new_x, new_y) == 0) {
      /* We cannot go to the next position, 
         in all cases we have to change it. 
         Or it is time to randomly change direction */
      ghost->direction = GetFreeDirection(ghost);
      ghost_same_dir[ghost->ghost_index] = 0;
   } else {
      ghost_same_dir[ghost->ghost_index]++;
      /* If we go in a single direction for a long time, randomly change it */
      if (ghost_same_dir[ghost->ghost_index] >= 8) {
	 ghost_same_dir[ghost->ghost_index] = 0;
	 ghost->direction = GetFreeDirection(ghost);
      }
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
      Ghost_Show(i,ghosts);
   }
}

/*
 * The ghost specified by ghostChar died
 */
void Ghost_Dies(Ghost *ghosts, unsigned char ghostChar) {
   unsigned char idx = GetIndexFromCharacter(ghostChar);
   if (idx <= (GHOSTS_NB-1)) {
      ghosts[idx].status = GHOST_DEAD;  
   }
}

#ifdef TEST

int testGhostMoves() {
   int testsInError = 0;
   
   Ghost ghost;
   
   ghost.position.x = 10;
   ghost.position.y = 10;
   ghost.direction = MOVES_RIGHT;
   Ghost_Move(&ghost);
   testsInError += assertEquals(11, ghost.position.x, "GM001");
   testsInError += assertEquals(10, ghost.position.y, "GM002");
   
   ghost.position.x = 10;
   ghost.position.y = 10;
   ghost.direction = MOVES_LEFT;
   Ghost_Move(&ghost);
   testsInError += assertEquals(9, ghost.position.x, "GM003");
   testsInError += assertEquals(10, ghost.position.y, "GM004");   

   ghost.position.x = 10;
   ghost.position.y = 10;
   ghost.direction = MOVES_UP;
   Ghost_Move(&ghost);
   testsInError += assertEquals(10, ghost.position.x, "GM005");
   testsInError += assertEquals(9, ghost.position.y, "GM006");   

   ghost.position.x = 10;
   ghost.position.y = 10;
   ghost.direction = MOVES_DOWN;
   Ghost_Move(&ghost);
   testsInError += assertEquals(10, ghost.position.x, "GM007");
   testsInError += assertEquals(11, ghost.position.y, "GM008");

   return testsInError;
}

int testGhostStatus() {
   int testsInError = 0;
   
   Ghost ghost;
   Ghost_SetStatus(&ghost, 0);
   testsInError += assertEquals(0, Ghost_GetStatus(&ghost), "GS001");

   Ghost_SetStatus(&ghost, 1);   
   testsInError += assertEquals(1, Ghost_GetStatus(&ghost), "GS002");
   
   return testsInError;
}


int testGhostShow() {
   int testsInError = 0;
   
   Ghost ghost[2];
   ghost[0].position.x = 0;
   ghost[0].position.y = 0;
   ghost[1].prevChar = COIN_LARGE;
   
   T6963C_writeAt(ghost[0].position.x, ghost[0].position.y, GHOST2_NORMAL);
   Ghost_Show(0, ghost);
   testsInError += assertEquals(COIN_LARGE, ghost[0].prevChar, "GSH001");
   
   T6963C_writeAt(ghost[0].position.x, ghost[0].position.y, PACMAN_UP_BODY1);
   Ghost_Show(0, ghost);
   testsInError += assertEquals(EMPTY, ghost[0].prevChar, "GSH002");
   
   T6963C_writeAt(ghost[0].position.x, ghost[0].position.y, COIN_SMALL);
   Ghost_Show(0, ghost);
   testsInError += assertEquals(COIN_SMALL, ghost[0].prevChar, "GSH003");  
   
   return testsInError;
}

int testGhostCanMove() {
   int testsInError = 0;
   
   T6963C_writeAt(10, 10, OBSTACLE_LEFT_DOWN);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM001");
   
   T6963C_writeAt(10, 10, OBSTACLE_LEFT_UP);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM001");
   
   T6963C_writeAt(10, 10, OBSTACLE_RIGHT_DOWN);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM002");
   
   T6963C_writeAt(10, 10, OBSTACLE_RIGHT_UP);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM003");
   
   T6963C_writeAt(10, 10, OBSTACLE_VERTICAL);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM004");
   
   T6963C_writeAt(10, 10, OBSTACLE_HORIZONTAL);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM005");
   
   T6963C_writeAt(10, 10, OBSTACLE_VERTICAL_LEFT);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM006");

   T6963C_writeAt(10, 10, OBSTACLE_VERTICAL_RIGHT);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM007");
   
   T6963C_writeAt(10, 10, OBSTACLE_HORIZONTAL_UP);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM008");

   T6963C_writeAt(10, 10, OBSTACLE_HORIZONTAL_DOWN);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM009");
 
   T6963C_writeAt(10, 10, GHOST1_NORMAL);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM010");
   
   T6963C_writeAt(10, 10, GHOST1_WEAK);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM011");

    T6963C_writeAt(10, 10, GHOST2_NORMAL);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM012");
   
   T6963C_writeAt(10, 10, GHOST2_WEAK);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM013");
 
   T6963C_writeAt(10, 10, GHOST3_NORMAL);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM014");
   
   T6963C_writeAt(10, 10, GHOST3_WEAK);
   testsInError += assertEquals(0, Ghost_CanMove(10,10), "GCM015"); 
   
   return testsInError;
}

int testGhost() {
   int testsInError = 0;
   
   testsInError += testGhostMoves();
   testsInError += testGhostCanMove();
   testsInError += testGhostStatus();
   testsInError += testGhostShow();
   
   return testsInError;
}

#endif
