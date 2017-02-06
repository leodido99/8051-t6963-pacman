#ifndef ___GHOSTS_H
#define ___GHOSTS_H

#include "pacman.h"

#define GHOST1_SPAWN_X 3
#define GHOST1_SPAWN_Y 2
#define GHOST2_SPAWN_X 3
#define GHOST2_SPAWN_Y 2
#define GHOST3_SPAWN_X 3
#define GHOST3_SPAWN_Y 2

typedef enum {
   GHOST_NORMAL,
   GHOST_WEAK,
   GHOST_DEAD
} GhostStatus;

typedef struct {
  unsigned char ghost_index;
  Direction direction;
  Position position;
  GhostStatus status;
} Ghost;

#define GHOSTS_NB 3

void Ghost_Dies(Ghost *ghosts, unsigned char ghostChar);
void Ghost_PlaceAll(Ghost *ghosts);
void Ghost_Iterate(Ghost *ghosts);
void Ghost_SetStatus(Ghost *ghost, unsigned int weak);

#endif
