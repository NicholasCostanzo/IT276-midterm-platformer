#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "sprite.h"
#include "mouse.h"
#include "level.h"
#include "graphics.h"
#include "entity.h"
#include "levelTree.h"
#include "ui.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;
extern Entity *player;
extern struct node *levelMap;
int done;
int drawMouse;
int mode;

void Init_All();

void updateCamera();
/*void mainMenu();*/


/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
  int done;
  int keyn;
  Uint8 *keys;
  
  Init_All();
  done = 0;
  drawMouse = 1;
  
  buildLevelTree();
  Button *b = newStartGameButton();
  Menu *m = newMainMenu(b);
  /*LoadLevel(numToFileName(levelMap->level)); 
  CreateLevelEntities();
  player = makePlayer();*/
  
  
  
  do
  {
    
    ResetBuffer();
    thinkEntityList();
    DrawLevel();
    drawEntityList();
    drawUiElements();
    updateOnClick();
    updateCamera();
    
    if(drawMouse == 1)DrawMouse();
    NextFrame();
    SDL_PumpEvents();
    keys = SDL_GetKeyState(&keyn);
    if(keys[SDLK_ESCAPE])done = 1;
  }while(!done);
  exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
  return 0;
}

void CleanUpAll()
{
  destroyTree(levelMap);
  CloseSprites();
  clearEntities();
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();
  initEntityList();
  InitLevelSystem();
  InitMouse();
  InitUi();  
  atexit(CleanUpAll);
}

/*
 *  TODO: split this off into ui.c
 */
/*void mainMenu()
{
  int keyn;
  int start = 0;
  Uint8 *keys;
  Sprite *bgpath = LoadSprite("images/background2.png",1024,768);
  Sprite *beginpath = LoadSprite("images/begin.png",467,53);
  while(!start)
  {
    ResetBuffer();
    DrawSprite(bgpath,screen,0,0,0);
    DrawSprite(beginpath,screen,100,100,0);
    keys = SDL_GetKeyState(&keyn);
    
    NextFrame();
    SDL_PumpEvents();
    if(keys[SDLK_SPACE]) start = 1;
  }
  
}*/

/*
 * TODO: split camera functionality off into camera.c and tweak this functionality
 */
void updateCamera()
{
  if(player)Camera.x = player->bbox.x - (Camera.w >> 1);
  else Camera.x = 0;
  if(player)Camera.y = player->bbox.y - (Camera.h >> 1);
  else Camera.y = 0;
  if(Camera.x > buffer->w - Camera.w)Camera.x = buffer->w - Camera.w;
  if(Camera.x < 0)Camera.x = 0;
  if(Camera.y > buffer->h - Camera.h)Camera.y = buffer->h - Camera.h;
  if(Camera.y < 0)Camera.y = 0;
}
