#ifndef ___GHOSTS_H
#define ___GHOSTS_H

#include "pacman.h"
#include "ghosts.h"

#define GHOST1_SPAWN_X GMB_MIDDLE_X - 2
#define GHOST1_SPAWN_Y GMB_MIDDLE_Y - 1
#define GHOST2_SPAWN_X GMB_MIDDLE_X + 1
#define GHOST2_SPAWN_Y GMB_MIDDLE_Y - 1
#define GHOST3_SPAWN_X GMB_MIDDLE_X
#define GHOST3_SPAWN_Y GMB_MIDDLE_Y

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
  unsigned char prevChar;
} Ghost;

#define GHOSTS_NB 3

void Ghost_Dies(Ghost *ghosts, unsigned char ghostChar);
void Ghost_PlaceAll(Ghost *ghosts);
void Ghost_Iterate(Ghost *ghosts);
void Ghost_SetStatus(Ghost *ghost, unsigned int weak);
unsigned int Ghost_GetStatus(Ghost *ghost);

#ifdef TEST
   int testGhost();
#endif

#endif
