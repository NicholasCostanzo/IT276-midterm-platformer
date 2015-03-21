#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "sprite.h"
#include "mouse.h"
#include "level.h"
#include "graphics.h"
#include "entity.h"
#include "levelTree.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;
extern Entity *player;
extern struct node *levelMap;

void Init_All();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
void addCoordinateToFile(char *filepath,int x, int y);
void updateCamera();


/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
  int done;
  int keyn;
  Uint8 *keys;
  
  Init_All();
  done = 0;
  
  buildLevelTree();
  LoadLevel(numToFileName(levelMap->level)); 
  CreateLevelEntities();
  player = makePlayer();
  
  preorder(levelMap); /*TODO: probably should remove this or wrap it in a debug state*/
  fprintf(stdout, "\n");
  
  do
  {
    ResetBuffer();
    thinkEntityList();
    DrawLevel();
    drawEntityList();
    updateCamera();
    
    /*DrawMouse();*/ /*No need to draw mouse right now*/
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
  fprintf(stdout, "exit? \n");
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();
  initEntityList();
  InitLevelSystem();
  /*InitMouse();*/
  atexit(CleanUpAll);
}

int getImagePathFromFile(char *filepath,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int returnValue = -1;
    if (!filepath)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    if (fscanf(fileptr,"%s",buf))
    {
        if (strcmp(buf,"image:")==0)
        {
            fscanf(fileptr,"%s",filepath);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    return returnValue;
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

void updateCamera()
{
  Camera.x = player->bbox.x - (Camera.w >> 1);
  Camera.y = player->bbox.y - (Camera.h >> 1);
  if(Camera.x > buffer->w - Camera.w)Camera.x = buffer->w - Camera.w;
  if(Camera.x < 0)Camera.x = 0;
  if(Camera.y > buffer->h - Camera.h)Camera.y = buffer->h - Camera.h;
  if(Camera.y < 0)Camera.y = 0;
}
