#include "ui.h"
#include "levelTree.h"
#include "level.h"
#include "entity.h"

#define __MaxButtons 256
#define __MaxMenus 256

extern SDL_Surface *screen;
extern struct node *levelMap;
extern Entity *player;
extern SDL_Rect Camera;
extern int drawMouse;
extern int mode;

Button ButtonList[__MaxButtons];
Menu MenuList[__MaxMenus];

void InitUi()
{
  int i;
  memset(ButtonList,0,sizeof(Button) * __MaxButtons);
  for(i = 0;i < __MaxButtons;i++)ButtonList[i].inuse = 0;
  memset(MenuList,0,sizeof(Menu) * __MaxMenus);
  for(i = 0;i < __MaxMenus;i++)MenuList[i].inuse = 0;
}

void CloseUi()
{
  
}

Button* newStartGameButton()
{
  int i;
  Sprite *img = LoadSprite("images/begin.png",467,53); /*TODO stop using magic numbers, you dingus!*/
  for (i = 0; i < __MaxButtons; i++)
  {
    if(ButtonList[i].inuse == 0)
    {
      ButtonList[i].inuse = 1;
      ButtonList[i].image = img;
      ButtonList[i].x = 200;
      ButtonList[i].y = 100;
      ButtonList[i].w = 467;
      ButtonList[i].h = 53;
      ButtonList[i].onClick = mainMenu;
      ButtonList[i].visible = 1;
      return &ButtonList[i];
    }
  }
  return NULL;
}

Button* newSpriteButton(int spr)
{
  int i;
  Sprite *img;
  int y = 100;
  int x = 100 + spr*50;
  
  if(spr==0)
  {
    img = LoadSprite("images/background2.png",32,32);
  }
  else if(spr==1)
  {
    img = LoadSprite("images/block1.png",32,32);
  }
  else if(spr==2)
  {
    img = LoadSprite("images/background2.png",32,32);
  }
  else if(spr==3)
  {
    img = LoadSprite("images/goal1.png",32,32); 
  }
  else if(spr==4)
  {
    img = LoadSprite("images/enemy3.png",32,32);
  }
  else if(spr==5)
  {
    img = LoadSprite("images/enemy2.png",32,32);
  }
  else if(spr==6)
  {
    img = LoadSprite("images/enemy1.png",32,32);
  }
  else if(spr==9)
  {
    img = LoadSprite("images/enemy9.png",32,32);
  }
  
  for (i = 0; i < __MaxButtons; i++)
  {
    if(ButtonList[i].inuse == 0)
    {
      ButtonList[i].inuse = 1;
      ButtonList[i].image = img;
      ButtonList[i].x = x;
      ButtonList[i].y = y;
      ButtonList[i].w = 32;
      ButtonList[i].h = 32;
      ButtonList[i].meta = spr; /*use meta int to store level tile type*/
      ButtonList[i].visible = 1;
      ButtonList[i].onClick = spriteOnClick;
      return &ButtonList[i];
    }
  }
  return NULL;
}

void spriteOnClick(Button *self)
{
  mode = self->meta;
  fprintf(stdout, "current mode: %i \n", mode);
}

Menu* newMenu(Sprite *img, Button *but, int x, int y)
{
  int i;
  for(i = 0; i < __MaxMenus; i++)
  {
    MenuList[i].inuse = 1;
    MenuList[i].image = img;
    MenuList[i].x = x;
    MenuList[i].y = y;
    MenuList[i].button = but;
  }
}

Menu* newMainMenu(Button *but)
{
  Sprite *img = LoadSprite("images/background2.png", 500, 80);
  return newMenu(img, but, 190, 80);
}

void drawUiElements()
{
  
  int i;
  for(i = 0; i < __MaxMenus; i++)
  {
    if(MenuList[i].inuse == 1)DrawSprite(MenuList[i].image, screen, MenuList[i].x - Camera.x, MenuList[i].y - Camera.y, 0);
  }
  for(i = 0; i < __MaxButtons; i++)
  {
    if(ButtonList[i].inuse == 1 && ButtonList[i].visible == 1)DrawSprite(ButtonList[i].image, screen, ButtonList[i].x - Camera.x, ButtonList[i].y - Camera.y, 0);
  }
}

void updateOnClick()
{
  int x, y, i;
  Uint8 *mouse = SDL_GetMouseState(&x,&y);
  if(SDL_GetMouseState(NULL,NULL) && SDL_BUTTON(SDL_BUTTON_LEFT))
  {
    for(i = 0; i < __MaxButtons; i++)
    {
      if(ButtonList[i].inuse == 1 && ButtonList[i].visible == 1 && ButtonList[i].x < x && ButtonList[i].y < y && ButtonList[i].x + ButtonList[i].w > x && ButtonList[i].y + ButtonList[i].h > y)
	ButtonList[i].onClick(&ButtonList[i]);
    }
  }
}

void mainMenu(Button *self)
{
  int i;
  LoadLevel(numToFileName(levelMap->level)); 
  CreateLevelEntities();
  player = makePlayer();
  preorder(levelMap); /*TODO: probably should remove this or wrap it in a debug state*/
  fprintf(stdout, "\n");
  self->inuse=0;
  drawMouse = 0;
  for(i = 0; i < __MaxMenus; i++){
   if(MenuList[i].button = self) MenuList[i].inuse = 0; 
  }
  /*int keyn;
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
  }*/
  
}