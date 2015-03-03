#include "entity.h"

#define __MaxEntities 4096

extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Surface *screen;

Entity * __entityList = NULL;
Entity EntityList[__MaxEntities];
Entity BlockEntityList[__MaxEntities];


int NumEntities;

void initEntityList()
{
    int x;
    NumEntities = 0;
    NumEntities = 0;
    memset(EntityList,0,sizeof(Entity) * __MaxEntities);
    for(x = 0;x < __MaxEntities;x++)EntityList[x].inuse = 0;
    /* load entity config from file...*/
    __entityList = (Entity *)malloc(sizeof(Entity)*__MaxEntities);
    if (__entityList == NULL)
    {
        fprintf(stderr,"initEntityList: FATAL: cannot allocate entity list\n");
        exit(-1);
        return;
    }
    memset(__entityList,0,sizeof(Entity)*__MaxEntities);
    
}

void closeEntityList();

void drawEntityList(){
  int i;
  for(i = 0; i < NumEntities; i++)
  {
    /*if(__entityList[i].inuse==1)
    {
      ent1 = &__entityList[i];
      drawEntity(ent1, buffer, 0);
    }*/
    if(EntityList[i].inuse==1)
    {
      drawEntity(&EntityList[i], screen, EntityList[i].frame);
    }
  }
}

void drawBlockEntityList()
{
  int i, frame;
  for(i = 0; i < __MaxEntities; i++){
    /*fprintf(stdout,"MADE IT HERE: %i\n", (BlockEntityList[i].inuse));*/
    frame = BlockEntityList[i].frame;
    drawEntity(&BlockEntityList[i], screen, frame);
    if(BlockEntityList[i].inuse==0)
      return;
  }
}

void drawEntity(Entity *ent, SDL_Surface *surface, int frame){
  if(ent->inuse==1) DrawSprite(ent->sprite, surface, ent->bbox.x, ent->bbox.y, frame);
}

void updateEntityList();
void thinkEntityList();

Entity *newEntity()
{
  int i;
    for (i = 0; i < __MaxEntities; i++)
    {
        /*if (__entityList[i].inuse == 0)
        {
            __entityList[i].inuse = 1;
	    NumEntities++;
            return &__entityList[i];
        }*/
	if(EntityList[i].inuse==0)
	{
	  EntityList[i].inuse = 1;
	  NumEntities++;
	  return &EntityList[i];
	}	
    }
    return NULL;
}

Entity *newBlockEntity(Sprite *spr, int x, int y, int w, int h, int frame){
  int i;
  for (i = 0; i < __MaxEntities; i++)
  {
    if(BlockEntityList[i].inuse == 0)
    {
      BlockEntityList[i].inuse = 1;
      BlockEntityList[i].frame = frame;
      BlockEntityList[i].sprite = spr;
      BlockEntityList[i].bbox.x = x;
      BlockEntityList[i].bbox.y = y;
      BlockEntityList[i].bbox.w = w;
      BlockEntityList[i].bbox.h = h;
      return &BlockEntityList[i];
    }
  }
    return NULL;
}

/*
 * this uses a bunnch of magic numbers just to get started. TODO: change to use variables
 */
Entity *makePlayer()
{
  Entity* player = newEntity();
  player->sprite = LoadSprite("images/player1.png",32,32);
  player->bbox.x = 100;
  player->bbox.y = 672;
  player->bbox.w = 32;
  player->bbox.h = 32;
  player->frame = 0;
  player->inuse = 1;
  return player;
}

void freeEntity(Entity **ent)
{
    /*handle freeing resources like Sprite data*/
    memset(*ent,0,sizeof(Entity));
    *ent = NULL;
}



/*eol@eof*/
