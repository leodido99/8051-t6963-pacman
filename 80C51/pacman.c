#include "pacman.h"
#include "gameboard.h"
#include "t6963c.h"
#include "ghosts.h"
#include "buffer.h"
#include "cherry.h"

#ifdef TEST
#include "test.h"
#endif

/* Used to know if the pacman must be erased this iteration */
unsigned char g_erase_pacman = 0;

/* Initialize the pacman structure */
void Pacman_Init(Pacman *pacman) {
   g_erase_pacman = 0;
   pacman->direction = MOVES_NEUTRAL;
   pacman->position.x = PACMAN_SPAWN_X;
   pacman->position.y = PACMAN_SPAWN_Y;
   pacman->status = ALIVE;
   pacman->lastBody = BODY2;
}

unsigned char Pacman_MovePossible(unsigned char x, unsigned y) {
   unsigned char possibleMove;
   unsigned char charAtPosition = T6963C_readFrom(x, y);
   switch(charAtPosition) {
      case OBSTACLE_LEFT_DOWN:
      case OBSTACLE_LEFT_UP:
      case OBSTACLE_RIGHT_DOWN:
      case OBSTACLE_RIGHT_UP:
      case OBSTACLE_VERTICAL:
      case OBSTACLE_VERTICAL_LEFT:
      case OBSTACLE_VERTICAL_RIGHT:
      case OBSTACLE_HORIZONTAL:
      case OBSTACLE_HORIZONTAL_UP:
      case OBSTACLE_HORIZONTAL_DOWN:
      case GHOST1_NORMAL:
      case GHOST2_NORMAL:
      case GHOST3_NORMAL:	 
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
	 if (new_x == PACMAN_LIMIT_X0) {
	    /* Leaving screen */
	    new_x = PACMAN_LIMIT_X1;
	 } else {
	    new_x--;
	 }
	 break;
      case MOVES_RIGHT:
	 if (new_x == PACMAN_LIMIT_X1) {
	    new_x = PACMAN_LIMIT_X0;
	 } else {
	    new_x++;
	 }
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
	 EventGhostsWeak();
	 break;
      case CHERRY:
	 pacman->points += CHERRY_POINTS;
	 Cherry_Place();
	 break;
      case GHOST1_WEAK:
      case GHOST2_WEAK:
      case GHOST3_WEAK:
	 /* Dead ghost */
	 EventGhostDies(charAtPosition);
	 pacman->points += GHOST_POINTS;
	 break;
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
   if (pacman->direction != MOVES_NEUTRAL) {
      /* Alternate between body 1 and body 2 */
      body = PACMAN_UP_BODY1 + pacman->direction * 2 + pacman->lastBody;
      /* Toggle body type */
      pacman->lastBody ^= 1; 
   } else {
      body = PACMAN_RIGHT_BODY1;
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
 * @param pacman La définition du serpent.
 * @param arrow La touche pressé par l'utilisateur
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

int testPacmanTurnsTo(Direction currentDirection, Arrow turn, Direction expectedResult, char *testCode) {
	Pacman pacman =	{MOVES_RIGHT, {10, 10}, ALIVE, 0, 0, BODY2};
	pacman.direction = currentDirection;
	Pacman_turn(&pacman, turn);
	return assertEquals(expectedResult, pacman.direction, testCode);	
}

int testPacmanTurn(void) {
   int testsInError = 0;
   testsInError += testPacmanTurnsTo(MOVES_RIGHT, ARROW_RIGHT,	MOVES_RIGHT, "ST01");
   testsInError += testPacmanTurnsTo(MOVES_RIGHT, ARROW_LEFT,	MOVES_LEFT,"ST02");
   testsInError += testPacmanTurnsTo(MOVES_RIGHT, ARROW_UP,	MOVES_UP, 	"ST03");
   testsInError += testPacmanTurnsTo(MOVES_RIGHT, ARROW_DOWN, 	MOVES_DOWN, "ST04");
   
   
   testsInError += testPacmanTurnsTo(MOVES_LEFT, ARROW_RIGHT, 	MOVES_RIGHT, "ST11");
   testsInError += testPacmanTurnsTo(MOVES_LEFT, ARROW_LEFT, 	MOVES_LEFT, "ST12");
   testsInError += testPacmanTurnsTo(MOVES_LEFT, ARROW_UP, 	MOVES_UP, 	"ST13");
   testsInError += testPacmanTurnsTo(MOVES_LEFT, ARROW_DOWN, 	MOVES_DOWN, "ST14");

   testsInError += testPacmanTurnsTo(MOVES_UP, ARROW_RIGHT, 	MOVES_RIGHT,"ST21");
   testsInError += testPacmanTurnsTo(MOVES_UP, ARROW_LEFT, 	MOVES_LEFT, "ST22");
   testsInError += testPacmanTurnsTo(MOVES_UP, ARROW_UP, 	MOVES_UP, 	"ST23");
   testsInError += testPacmanTurnsTo(MOVES_UP, ARROW_DOWN, 	MOVES_DOWN, 	"ST24");

   testsInError += testPacmanTurnsTo(MOVES_DOWN, ARROW_RIGHT, 	MOVES_RIGHT,"ST31");
   testsInError += testPacmanTurnsTo(MOVES_DOWN, ARROW_LEFT, 	MOVES_LEFT, "ST32");
   testsInError += testPacmanTurnsTo(MOVES_DOWN, ARROW_UP, 	MOVES_UP, "ST33");
   testsInError += testPacmanTurnsTo(MOVES_DOWN, ARROW_DOWN, 	MOVES_DOWN, "ST34");
   return testsInError;
}

int testPacmanEats() {
	int testsInError = 0;
	Pacman pacman = {MOVES_RIGHT, {BDD_SCREEN_X, BDD_SCREEN_Y}, ALIVE, 0, 0, BODY2};
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, CHERRY);
	Pacman_liveOrDie(&pacman);

	testsInError += assertEquals(pacman.status, ALIVE, "SF001A");
	testsInError += assertEquals(pacman.points, CHERRY_POINTS, "SF002A");

	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, COIN_SMALL);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);

	testsInError += assertEquals(pacman.status, ALIVE, "SF001B");
	testsInError += assertEquals(pacman.points, COIN_SMALL_POINTS, "SF002B");	
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, COIN_LARGE);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SF001C");
	testsInError += assertEquals(pacman.points, COIN_LARGE_POINTS, "SF002C");	
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST1_WEAK);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SF001D");
	testsInError += assertEquals(pacman.points, GHOST_POINTS, "SF002D");	
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST2_WEAK);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SF001E");
	testsInError += assertEquals(pacman.points, GHOST_POINTS, "SF002E");

	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST3_WEAK);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SF001F");
	testsInError += assertEquals(pacman.points, GHOST_POINTS, "SF002F");
		
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST1_NORMAL);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, DEAD, "SF001G");
	testsInError += assertEquals(pacman.points, 0, "SF002G");
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST2_NORMAL);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, DEAD, "SF001G");
	testsInError += assertEquals(pacman.points, 0, "SF002G");

	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, GHOST3_NORMAL);
	pacman.points = 0;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, DEAD, "SF001G");
	testsInError += assertEquals(pacman.points, 0, "SF002G");
	
	T6963C_writeAt(BDD_SCREEN_X, BDD_SCREEN_Y, EMPTY);
	pacman.points = 0;
	pacman.status = ALIVE;
	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SF003");

	return testsInError;	
}

int testPacmanMoves() {
	int testsInError = 0;
	Pacman pacman;

	pacman.position.x = 10;
	pacman.position.y = 10;

	pacman.direction = MOVES_UP;
	Pacman_move(&pacman);
	testsInError += assertEquals(9, pacman.position.y, "SM001");

	pacman.direction = MOVES_DOWN;
	Pacman_move(&pacman);
	testsInError += assertEquals(10, pacman.position.y, "SM002");

	pacman.direction = MOVES_LEFT;
	Pacman_move(&pacman);
	testsInError += assertEquals( 9, pacman.position.x, "SM003");

	pacman.direction = MOVES_RIGHT;
	Pacman_move(&pacman);
	testsInError += assertEquals(10, pacman.position.x, "SM004");

	return testsInError;
}

int testPacmanHitsABorder() {
	int testsInError = 0;

	Pacman pacman;
   	pacman.status = ALIVE;
	pacman.position.x = 10;
	pacman.position.y = 10;

	T6963C_writeAt(10, 11, OBSTACLE_VERTICAL);
        T6963C_writeAt(10, 9, OBSTACLE_VERTICAL);
   	T6963C_writeAt(11, 10, OBSTACLE_VERTICAL);
        T6963C_writeAt(9, 10, OBSTACLE_VERTICAL);
   
	pacman.direction = MOVES_LEFT;
 	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SO01A");  
	Pacman_move(&pacman);
	testsInError += assertEquals(pacman.direction,  MOVES_LEFT, "SO02A");
 	testsInError += assertEquals(pacman.position.x,  10, "SO03A");
	testsInError += assertEquals(pacman.position.y, 10, "SO04A");  

	pacman.direction = MOVES_RIGHT;
 	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SO01A");  
	Pacman_move(&pacman);
	testsInError += assertEquals(pacman.direction,  MOVES_RIGHT, "SO02A");
 	testsInError += assertEquals(pacman.position.x,  10, "SO03A");
	testsInError += assertEquals(pacman.position.y, 10, "SO04A");  

	pacman.direction = MOVES_DOWN;
 	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SO01A");  
	Pacman_move(&pacman);
	testsInError += assertEquals(pacman.direction,  MOVES_DOWN, "SO02A");
 	testsInError += assertEquals(pacman.position.x,  10, "SO03A");
	testsInError += assertEquals(pacman.position.y, 10, "SO04A");  

	pacman.direction = MOVES_UP;
 	Pacman_liveOrDie(&pacman);
	testsInError += assertEquals(pacman.status, ALIVE, "SO01A");  
	Pacman_move(&pacman);
	testsInError += assertEquals(pacman.direction,  MOVES_UP, "SO02A");
 	testsInError += assertEquals(pacman.position.x,  10, "SO03A");
	testsInError += assertEquals(pacman.position.y, 10, "SO04A");  

	return testsInError;
}


int testPacman() {
   int testsInError = 0;
   
   testsInError += testPacmanTurn();
   testsInError += testPacmanMoves();
   testsInError += testPacmanEats();
   testsInError += testPacmanHitsABorder();
   
   
   return testsInError;
}

#endif
