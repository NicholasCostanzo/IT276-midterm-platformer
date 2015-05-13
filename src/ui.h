#ifndef __Ui__
#define __Ui__

#include "sprite.h"

typedef struct Button_T
{
  int inuse;
  int x,y,w,h;
  int meta; /*additional integer value for varied potential use*/
  int visible; /*determines if active button is visible or not*/
  Sprite *image; /*image used for the button. TODO add font support so each button doesn't have to be made by hand*/
  void (*onClick)(struct Button_T* self); /*function to be called when button is clicked*/
} Button;

typedef struct Menu_T
{
  int inuse;
  int x,y;
  Sprite *image; /*image for the menu area. defines bbox, etc*/
  Button *button; /*button used in the menu area. TODO make this an array of buttons*/
} Menu;

void InitUi();
void CloseUi();
Menu* newMenu(Sprite *img, Button *but, int x, int y);
Menu* newMainMenu(Button *but);
Button* newStartGameButton();
Button* newSpriteButton(int spr);
void spriteOnClick(Button *self);
void drawUiElements();
void updateOnClick();
void mainMenu();

#endif
