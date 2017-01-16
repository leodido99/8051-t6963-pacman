#ifndef ___PACMAN_H
#define ___PACMAN_H


typedef enum {
	MOVES_UP,
	MOVES_DOWN,
	MOVES_LEFT,
	MOVES_RIGHT
} Direction;

typedef enum {
	ALIVE,
	DEAD,
	EATING
} Status;

typedef struct {
	Direction direction;
	Position position;
	Status status;
	unsigned char livesLeft;
	unsigned int points;
} Pacman;

void Pacman_move(Pacman *pacman);
void Pacman_turn(Pacman *pacman, Arrow arrow);


#endif