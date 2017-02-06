#include "cherry.h"
#include "gameboard.h"
#include "t6963c.h"
#include <stdlib.h>

/**
 * Returns a random free position to place the fruit
 */
void Cherry_GetRandomPosition(Position* position) {
   position->x = rand() % PACMAN_LIMIT_X1;
   position->y = rand() % PACMAN_LIMIT_Y1;
   /* Prevent a position on the border */
   if (position->y == 0) {
      position->y++;
   }  
}

/**
 * Place the cherry at the free position
 */
void Cherry_PlaceInFreeSpace(Position* position) {
   unsigned char x = position->x, y = position->y;
   /* Loop until we find a free cell */
   while(T6963C_readFrom(position->x, position->y) != EMPTY) {
      /* Update position, if we have some room go right, otherwise reset to 0 */
      if (position->x <= PACMAN_LIMIT_X1) {
	 position->x++;
      } else {
	 position->x = PACMAN_LIMIT_X0;
      }
   }
   /* Draw the cherry */
   T6963C_writeAt(position->x, position->y, CHERRY);
}

/**
 * Place a cherry in a random position
 */
void Cherry_Place(void) {
   Position position;
   Cherry_GetRandomPosition(&position);
   Cherry_PlaceInFreeSpace(&position);
}
