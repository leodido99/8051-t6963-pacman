#include <string.h>
#include "main.h"
#include "t6963c.h"
#include "bdd.h"
#include "gameboard.h"

#define ROM_CG_ADDRESS 0x0000

/**
 * Copie la définition d'un caractère depuis la ROM vers la zone de définition
 * des caractères du T6963C.
 * @param positionInRom Position du caractère dans la ROM.
 * @param cgCode Code du caractère dans le t6963c.
 */
void GMB_copyFromRomToCg(unsigned char positionInRom, unsigned char cgCode) {
	unsigned char *rom_cg_address = (unsigned char __xdata *) (ROM_CG_ADDRESS + positionInRom * 8);
	unsigned int video_cg_address = cgCode * 8 + T6963C_ZONE_CG;
	T6963C_autoWrite(video_cg_address, rom_cg_address, 8);
}

/**
 * Initialise les caractères utilisés pendant le jeu.
 */
void GMB_initialize() {
	GMB_copyFromRomToCg( 0, PACMAN_UP_BODY1);
	GMB_copyFromRomToCg( 1, PACMAN_UP_BODY2);
   	GMB_copyFromRomToCg( 2, PACMAN_DOWN_BODY1);
	GMB_copyFromRomToCg( 3, PACMAN_DOWN_BODY2);
   	GMB_copyFromRomToCg( 4, PACMAN_LEFT_BODY1);
	GMB_copyFromRomToCg( 5, PACMAN_LEFT_BODY2);
   	GMB_copyFromRomToCg( 6, PACMAN_RIGHT_BODY1);
	GMB_copyFromRomToCg( 7, PACMAN_RIGHT_BODY2);
   
	GMB_copyFromRomToCg( 8, OBSTACLE_LEFT_DOWN);
	GMB_copyFromRomToCg( 9, OBSTACLE_LEFT_UP);
	GMB_copyFromRomToCg( 10, OBSTACLE_RIGHT_DOWN);
	GMB_copyFromRomToCg( 11, OBSTACLE_RIGHT_UP); 
	GMB_copyFromRomToCg( 12, OBSTACLE_VERTICAL);
	GMB_copyFromRomToCg( 13, OBSTACLE_HORIZONTAL);

	GMB_copyFromRomToCg( 14, GHOST1_NORMAL);
	GMB_copyFromRomToCg( 15, GHOST1_WEAK);
   	GMB_copyFromRomToCg( 14, GHOST2_NORMAL);
	GMB_copyFromRomToCg( 15, GHOST2_WEAK);
	GMB_copyFromRomToCg( 14, GHOST3_NORMAL);
	GMB_copyFromRomToCg( 15, GHOST3_WEAK);
}

/**
 * Returns true if the provided position is free to move to
 * or false otherwise
 */
unsigned char GMB_MovePossible(unsigned char x, unsigned y) {
   unsigned char possibleMove;
   unsigned char charAtPosition = T6963C_readFrom(x, y);
   switch(charAtPosition) {
      case OBSTACLE_LEFT_DOWN:
      case OBSTACLE_LEFT_UP:
      case OBSTACLE_RIGHT_DOWN:
      case OBSTACLE_RIGHT_UP:
      case OBSTACLE_VERTICAL:
      case OBSTACLE_HORIZONTAL:
	 possibleMove = 0;
	 break;
      default:
	 possibleMove = 1;
	 break;
   }
   return possibleMove;
}

/**
 * Draws the complete level on the display
 */
void GMB_drawLevel(void) {
      /* Todo */
}

/**
 * Dessine un rectangle entre les coordonnées spécifiées.
 * Le carré est dessiné avec des caractères OBSTACLE_*, pour
 * que le serpent ne puisse pas le traverser.
 * @param x0, y0: Coordonnées de l'angle supérieur droit.
 * @param x1, y1: Coordonnées de l'angle inférieur gauche.
 */
void GMB_draw(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1) {
    unsigned char i;
   /*   A = x0,y0
        "ABBBBBBBBC",
        "D........E",
        "D........E",
        "D........E",
        "FGGGGGGGGH"
   #define BDD_SCREEN_X 19
   #define BDD_SCREEN_Y 10
   #define BDD_SCREEN_WIDTH 10
   #define BDD_SCREEN_HEIGHT 5   
   BDD_SCREEN_X, BDD_SCREEN_Y, BDD_SCREEN_X + BDD_SCREEN_WIDTH - 1, BDD_SCREEN_Y + BDD_SCREEN_HEIGHT - 1
   */
      
   
   T6963C_writeAt(x0, y0, OBSTACLE_LEFT_UP);
   for(i = x0+1; i < x1; i++) {
      T6963C_writeAt(i, y0, OBSTACLE_HORIZONTAL);
      T6963C_writeAt(i, y1, OBSTACLE_HORIZONTAL);
   }
   T6963C_writeAt(x1, y0, OBSTACLE_RIGHT_UP);
   for(i = y0+1; i < y1; i++) {
      T6963C_writeAt(x0, i, OBSTACLE_VERTICAL);
      T6963C_writeAt(x1, i, OBSTACLE_VERTICAL);
   }
   T6963C_writeAt(x1, y1, OBSTACLE_RIGHT_DOWN);
   T6963C_writeAt(x0, y1, OBSTACLE_LEFT_DOWN);
   
}

/**
 * Remplit avec des espaces le rectangle défini par les coordonnées.
 * Permet de nettoyer l'intérieur du rectangle dessiné avec GMB_draw.
 * @param x0, y0: Coordonnées de l'angle supérieur droit.
 * @param x1, y1: Coordonnées de l'angle inférieur gauche.
 */
void GMB_clear(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1) {
   unsigned char i, j;
   for(i = x0; i <= x1; i++) {
      for(j = y0; j <= y1; j++) {
	 T6963C_writeAt(i, j, EMPTY);
      }
   }
}

/**
 * Affiche un texte entouré d'un rectangle.
 * La largeur du rectangle est définie par la longueur du texte.
 * La fonction ne vérifie pas si le texte dépasse les bords de l'écran.
 * @param x0, y0 L'angle supérieur droit du rectangle.
 * @param text Le texte à afficher.
 */
void GMB_display(unsigned char x0, unsigned char y0, char *text) {
   unsigned char n;
   unsigned char txt_size = strlen(text);
  
   unsigned char x1 = x0 + txt_size + 1;
   unsigned char y1 = y0 + 2;
   
   GMB_draw(x0, y0, x1, y1);

   for(n = 0; n < txt_size; n++) {
	  T6963C_writeAt(x0 + 1 + n, y0 + 1, text[n] - 32); 
   }
   
}

#ifdef TEST
int bddGameboardDraw() {
	BddExpectedContent c = {
		"ABBBBBBBBC",
		"D........E",
		"D........E",
		"D........E",
		"FGGGGGGGGH"
	};

	BDD_clear();
	GMB_draw(BDD_SCREEN_X, BDD_SCREEN_Y, BDD_SCREEN_X + BDD_SCREEN_WIDTH - 1, BDD_SCREEN_Y + BDD_SCREEN_HEIGHT - 1);
	return BDD_assert(c, "GMBD");
}

int bddGameboardClear() {
	BddExpectedContent c = {
		"..........",
		".        .",
		".        .",
		".        .",
		".........."
	};

	BDD_clear();
	GMB_clear(BDD_SCREEN_X + 1, BDD_SCREEN_Y + 1, BDD_SCREEN_X + BDD_SCREEN_WIDTH - 2, BDD_SCREEN_Y + BDD_SCREEN_HEIGHT - 2);
	return BDD_assert(c, "GMBC");
}

int bddGameboardDisplay() {
	BddExpectedContent c = {
		"..........",
		".ABBBBBC..",
		".D TXT E..",
		".FGGGGGH..",
		".........."
	};

	BDD_clear();
	GMB_display(BDD_SCREEN_X + 1, BDD_SCREEN_Y + 1, " TXT ");
	return BDD_assert(c, "GMBT");
}

int testGameboard() {
	int testsInError = 0;

	testsInError += bddGameboardDraw();
	testsInError += bddGameboardClear();
	testsInError += bddGameboardDisplay();

	return testsInError;
}
#endif
