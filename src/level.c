#include "level.h"
#include "entity.h"
#include <string.h>
#include <stdio.h>

static Level __level;
extern SDL_Surface *screen;

void LoadLevel(char *filename)
{
  FILE *levelfile = NULL;
  char buf[128];
  char bgimagepath[128];
  int w,h;
  int i;
  char tilesetpath[128];
  char tilegoalpath[128];
  int tsw,tsh; /*tile set width and height*/
  int tw,th; /*tile map width and height*/
  int *maptemp = NULL;
  Sprite *temp, *temp2, *temp3;
  levelfile = fopen(filename,"r");
  if (levelfile == NULL)
  {
    fprintf(stdout,"LoadLevel: ERROR, could not open file: %s\n",filename);
    return;
  }
  while(fscanf(levelfile, "%s", buf) != EOF)
  {
    if (buf[0] == '#')
    {
      /*ignore comments*/
      fgets(buf, sizeof(buf), levelfile);
    }
    else if (strncmp(buf,"image:",128)==0)
    {
      fscanf(levelfile,"%s",bgimagepath);
    }
    else if (strncmp(buf,"width:",128)==0)
    {
      fscanf(levelfile,"%i",&w);      
    }
    else if (strncmp(buf,"height:",128)==0)
    {
      fscanf(levelfile,"%i",&h);      
    }
    else if (strncmp(buf,"tile_width:",128)==0)
    {
      fscanf(levelfile,"%i",&tw);
    }
    else if (strncmp(buf,"tile_height:",128)==0)
    {
      fscanf(levelfile,"%i",&th);
    }
    else if (strncmp(buf,"tile_image_frame_width:",128)==0)
    {
      fscanf(levelfile,"%i",&tsw);      
    }
    else if (strncmp(buf,"tile_image_frame_height:",128)==0)
    {
      fscanf(levelfile,"%i",&tsh);      
    }
    else if (strncmp(buf,"tile_image:",128)==0)
    {
      fscanf(levelfile,"%s",tilesetpath);
    }
    else if (strncmp(buf,"goal_image:",128)==0)
    {
      fscanf(levelfile,"%s",tilegoalpath);
    }
    else
    {
      /*eat up the rest of this line, and ignore it*/
      fgets(buf, sizeof(buf), levelfile);
    }
  }
  /*loaded all size info*/
  if ((tw > 0)&&(th > 0))
  {
    maptemp = (int *)malloc(sizeof(int)*tw*th);
    if (maptemp == NULL)
    {
      fprintf(stdout,"LoadLevel: ERROR, could NOT (FUCK YOU CARAMEL) allocate map data: FUCK\n");
      return;
    }
    rewind(levelfile);
    while(fscanf(levelfile, "%s", buf) != EOF)
    {
      if (strncmp(buf,"tile_data:",128)==0)
      {
        /*parse out the raw tile data*/
        for (i = 0;i < tw*th; ++i)
        {
          if (fscanf(levelfile, "%i", &maptemp[i]) == EOF)
          {
            /*hit end of file early!*/
            break;
          }
        }
        break;
      }
      else
      {
        /*eat up the rest of this line, and ignore it*/
        fgets(buf, sizeof(buf), levelfile);
      }
    }
  }
  fclose(levelfile);
  temp = LoadSprite(bgimagepath,w,h);
  temp2 = LoadSprite(tilesetpath,tsw,tsh);
  temp3 = LoadSprite(tilegoalpath, tsw, tsh);
  
  if (!temp)
  {
    fprintf(stdout,"LoadLevel: ERROR, could NOT (FUCK YOU CARAMEL) open sprite file: %s\n",bgimagepath);
    free(maptemp);
    return;
  }
  
  if (!temp)
  {
    fprintf(stdout,"LoadLevel: ERROR, could NOT (FUCK YOU CARAMEL) open sprite file: %s\n",tilesetpath);
    FreeSprite(temp);/*cleanup*/
    free(maptemp);
    return;
  }
  CloseLevel();
  __level.bgImage = temp;
  __level.loaded = 1;
  __level.tileSet = temp2;
  __level.tileGoal = temp3;
  __level.tileMap = maptemp;
  __level.tileWidth = tw;
  __level.tileHeight = th;
}

char* numToFileName(int num)
{
  static char lev [12];
  sprintf(lev, "levels/level%i", num);
  return lev;
}

void CreateLevelEntities()
{
  int i,j,x,y,w,h,tsw,tsh,frame, goalCount;
  goalCount=0;
  
  if (!__level.loaded)return;
  
  if ((__level.tileMap)
    && (__level.tileSet ))
  {
    for (j = 0;j < __level.tileHeight;++j)
    {
      for (i = 0;i < __level.tileWidth;++i)
      {
        if (__level.tileMap[(j * __level.tileWidth) + i] == 1)
        {
	  tsw = __level.tileSet->w;
	  tsh = __level.tileSet->h;
	  x = i * tsw;
	  y = j * tsh;
	  w = __level.tileWidth;
	  h = __level.tileHeight;
	  
	  frame = __level.tileMap[(j * __level.tileWidth) + i] - 1;
	  newBlockEntity(__level.tileSet, x, y, tsw, tsh, frame);
        }
        else if (__level.tileMap[(j * __level.tileWidth) + i] == 2)
	{
	  Entity* temp;
	  x = i * __level.tileSet->w;
	  y = j * __level.tileSet->h;
	  w = __level.tileSet->w;
	  h = __level.tileSet->h;
	  temp = newEntity();
	  temp->bbox.x = x;
	  temp->bbox.y = y;
	  temp->bbox.w = w;
	  temp->bbox.h = h;
	  temp->canKill = 1;
	}
	else if (__level.tileMap[(j * __level.tileWidth) + i] == 3)
	{
	  Entity* temp;
	  
	  goalCount++;
	  x = i * __level.tileSet->w;
	  y = j * __level.tileSet->h;
	  w = __level.tileSet->w;
	  h = __level.tileSet->h;
	  temp = newEntity();
	  temp->bbox.x = x;
	  temp->bbox.y = y;
	  temp->bbox.w = w;
	  temp->bbox.h = h;
	  temp->levelGoal = goalCount; /*1 if first goal found, 2 if second, 3 if third, etc.*/
	  temp->sprite = __level.tileGoal;
	}

      }
    }
  }
}

void DrawLevel()
{
  if (__level.bgImage)
  {
    DrawSprite(__level.bgImage,screen,0,0,0);
  }
  drawBlockEntityList();
  /*CreateLevelEntities();*/
}

void CloseLevel()
{
  if (!__level.loaded)return;
  if (__level.bgImage != NULL)
  {
    FreeSprite(__level.bgImage);
    __level.bgImage = NULL;
  }
  if (__level.tileSet != NULL)
  {
    FreeSprite(__level.tileSet);
    __level.tileSet = NULL;
  }
  
  /*TODO: this is causing a seg fault??? figure out why*/
  /*if (__level.tileGoal != NULL) 
  {
    free(__level.tileGoal);
    __level.tileGoal = NULL;
  }*/
  if (__level.tileMap != NULL)
  {
    free(__level.tileMap);
    __level.tileMap = NULL;
  }
}

void InitLevelSystem()
{
  memset(&__level,0,sizeof(Level));
  atexit(CloseLevel);
}



/*eol@eof*/
