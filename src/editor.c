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
int done; /*used to exit the game loop*/
int drawMouse; /*do we draw the mouse or not?*/
int mode; /*what draw mode are we in?*/
int vis; /*is the draw menu visible?*/
int grid[32][24]; /*grid (currently only supports one screen length) to contain level data*/
Button *b[10]; /*array of buttons to display in the draw menu*/

void Init_All();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
void addCoordinateToFile(char *filepath,int x, int y);
void updateCamera();
void initEditorMenu(); /*set up the buttons needed for the level editor menu to function*/
void showEditorMenu(); /*set the visibility of the level editor menu to true*/
void hideEditorMenu(); /*set the visibility of the level editor menu to false*/
void updateMenuVisibility(); /*checks if the menu should be shown or not, and updates the editor accordingly*/
void drawLevelGrid(); /*draw the contents of the single-screen level room*/
void outputLevelToFile(char *filepath); /*write the current contents of the level grid to a file*/
/*TODO consider splitting off a bunch of these functions into their own file*/


/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
  int keyn, mx, my;
  Uint8 *keys;
  
  Init_All();
  done = 0;  
  drawMouse = 1;
  mode = 1; /*what block type is going to be drawn 0=air 1=block 3=exit other=enemy*/
  
  do
  {
    ResetBuffer();
    thinkEntityList();
    DrawLevel();
    drawEntityList();
    if(SDL_GetMouseState(&mx, &my) && SDL_BUTTON(SDL_BUTTON_LEFT) && vis == 0){grid[mx/32][my/32] = mode;} /*update the grid based on mouse clicks*/
    drawLevelGrid();
    drawUiElements();
    updateMenuVisibility();
    updateOnClick();
    updateCamera();
    
    DrawMouse(); /*No need to draw mouse right now*/
    NextFrame();
    SDL_PumpEvents();
    keys = SDL_GetKeyState(&keyn);
    if(keys[SDLK_s]) outputLevelToFile("editorlevel");
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
  initEditorMenu();
  atexit(CleanUpAll);
}

void initEditorMenu()
{
  int i;
  for(i = 0; i <= 9; i++)
  {
    b[i] = newSpriteButton(i);
  }
}

void showEditorMenu()
{
  int i;
  for(i = 0; i <= 9; i++)
  {
    b[i]->visible = 1;
  }
}

void hideEditorMenu()
{
  int i;
  for(i = 0; i <= 9; i++)
  {
    b[i]->visible = 0;
  }
}

void updateMenuVisibility()
{
  vis = 0;
  int numkeys;
  Uint8 *keys = SDL_GetKeyState(&numkeys);   
  if(keys[SDLK_SPACE])
  {
    vis = 1;
  }
  if(vis == 1)
  {
    showEditorMenu();
  }
  else hideEditorMenu();
}

void drawLevelGrid()
{
  int i, j;
  for(i = 0; i < 32; i++)
  {
    for(j = 0; j < 24; j++)
    {
      DrawSprite(b[grid[i][j]]->image, screen, i * 32,j * 32, 0);
    }
  }
}

outputLevelToFile(char *filepath)
{  
  int i, j;
  FILE *fileptr = NULL;
  
  if(!filepath)
  {
    fprintf(stdout, "outputLevelToFile: warning, no output file provided.\n");
    return;
  }
  fileptr = fopen(filepath, "w"); /*create for writing so file is updated each time instead of being added to*/
  if(!fileptr)
  {
    fprintf(stdout, "unable to open file: %s\n", filepath);
    return;
  }
  
  /*image: images/background1.png
    width: 1024
    height: 768
    tile_width: 32
    tile_height: 24
    tile_image: images/block1.png
    goal_image: images/goal1.png
    tile_image_frame_width: 32
    tile_image_frame_height: 32
    tile_data:*/
  
  fprintf(fileptr,"width: 1024\nheight: 768\ntile_width: 32\ntile_height: 24\ntile_image: images/block1.png\ngoal_image: images/goal1.png\ntile_image_frame_width: 32\ntile_image_frame_height: 32\ntile_data:\n");
  
  for(j = 0; j < 24; j++)
  {
    for(i = 0; i < 32; i++)
    {
      fprintf(stdout, "file opened: %i %i\n", i, j);
      fprintf(fileptr,"%i ", grid[i][j]);
    }
    fprintf(fileptr,"\n");
  }
  
  fprintf(fileptr,"tile_data_end\n");
}

void addCoordinateToFile(char *filepath,int x, int y)
{
    FILE *fileptr = NULL;
    if (!filepath)
    {
        fprintf(stdout,"addCoordinateToFile: warning, no input file path provided\n");
        return;
    }
    fileptr = fopen(filepath,"a");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filepath);
        return;
    }
    fprintf(fileptr,"newcoordinate: %i %i\n",x,y);
    fclose(fileptr);
}

int getCoordinatesFromFile(int *x, int *y,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int tx,ty;
    int returnValue = -1;
    if ((!x)&&(!y))
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    while (fscanf(fileptr,"%s",buf) != EOF)
    {
        /*fprintf(stdout,"buf is: %s\n",buf);*/
        if (strcmp(buf,"position:")==0)
        {
            fscanf(fileptr,"%i %i",&tx,&ty);
            fprintf(stdout,"as read: %i, %i\n",tx,ty);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    if (x)*x = tx;
    if (y)*y = ty;
    return returnValue;
}


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
