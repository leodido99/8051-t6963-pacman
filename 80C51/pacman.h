#ifndef ___PACMAN_H
#define ___PACMAN_H

#include "main.h"
#include "keyboard.h"

typedef enum {
   BODY1,
   BODY2
} BodyType;

typedef enum {
	MOVES_UP,
	MOVES_DOWN,
	MOVES_LEFT,
	MOVES_RIGHT
} Direction;

typedef enum {
	ALIVE,
	DEAD,
	POWERUP,
	BERZERK
} Status;

typedef struct {
	Direction direction;
	Position position;
	Status status;
	unsigned char livesLeft;
	unsigned int points;
	BodyType lastBody;
} Pacman;

Status Pacman_iterate(Pacman *pacman, Arrow arrow);
void Pacman_move(Pacman *pacman);
void Pacman_turn(Pacman *pacman, Arrow arrow);
void Pacman_showHead(Pacman *pacman);
void Pacman_liveOrDie(Pacman *pacman);


#endif