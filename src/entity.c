#include "entity.h"
#include "sprite.h"

#define MaxEntities 255

extern SDL_Surface *buffer; /*pointer to the draw buffer*/

int NumEntities;
Entity EntityList[MaxEntities];

void InitEntityList()
{
  int x;
  NumEntities = 0;
  memset(EntityList,0,sizeof(Entity) * MaxEntities);
  for(x = 0;x < MaxEntities;x++)EntityList[x].inuse = 0;
}

Entity *NewEntity(Sprite *sprite, int xpos, int ypos)
{
  int i;
  /*makesure we have the room for a new entity*/
  if(NumEntities + 1 >= MaxEntities)
  {
        fprintf(stderr, "Maximum Entities Reached.\n");
        exit(1);
  }
  
  
  /*Add the entity to the list*/
  for(i = 0; i < NumEntities; i++)
  {
    if(EntityList[i].inuse==0)
    {
      EntityList[i].inuse++;
      break;
    }
  }
  NumEntities++;
  
  EntityList[i].sprite = sprite;
  EntityList[i].xpos = xpos;
  EntityList[i].ypos = ypos;
  
  return &EntityList[i];
}

void DrawEntityList(){
  int i = 0;
  for(i = 0; i < NumEntities; i++)
  {
    if(EntityList[i].inuse==1) 
      DrawSprite(EntityList[i].sprite, buffer, EntityList[i].xpos, EntityList[i].ypos, 0);
  }
}

void DrawEntity(Entity *ent){
  if(ent->inuse==1) DrawSprite(ent->sprite, buffer, ent->xpos, ent->ypos, 0);
}

void FreeEntity(Entity *ent)
{
  ent->inuse--;
  if(ent->inuse == 0)
  {
    ent->sprite = NULL;
    ent->xpos = NULL;
    ent->ypos = NULL;
  }
}
