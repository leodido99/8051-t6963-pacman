#include "pacman.h


/**
 * Modifie les coordonnées du serpent selon sa direction.
 * @param snake La description du serpent.
 */
void Pacman_move(Pacman *pacman) {
   /* TODO check for colision before moving */
   switch(pacman->direction) {
      case MOVES_DOWN:
	 pacman->position.y++;
	 break;
      case MOVES_UP:
	 pacman->position.y--;
	 break;      
      case MOVES_LEFT:
	 pacman->position.x--;
	 break;
      case MOVES_RIGHT:
	 pacman->position.x++;
	 break;       
   }
}

/**
 * Décide de varier la direction du serpent selon la direction indiquée.
 * Le serpent ne peut jamais tourner de 180º en un mouvement.
 * @param snake La description du serpent.
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

void Pacman_liveOrDie(Pacman *pacman) {

}

void Pacman_showHead(Pacman *pacman) {

   
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
