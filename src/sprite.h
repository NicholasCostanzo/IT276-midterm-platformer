#ifndef __SPRITE__
#define __SPRITE__

#include "graphics.h"
#include <SDL.h>
#include "SDL_image.h"
#include "SDL_ttf.h"

#define MAXSPRITEFILENAME 80

typedef struct Sprite_T
{
  SDL_Surface *image;
  char filename[MAXSPRITEFILENAME];
  int w, h;
  int framesperline;
  int color1, color2, color3;
  int used;
} Sprite;

void InitSpriteList();
void FreeSprite(Sprite *img);
Sprite *LoadSprite(char *filename,int sizex, int sizey);		/*simplest file to load*/
Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3);

void DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame);
void CloseSprites();		/*call this before you exit to make sure all the memory that your program used is given back to your OS*/

#endif