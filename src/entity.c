#include entity.h
#include sprite.h

#define MaxEntities 255

int NumEntities;
Entity EntityList[MaxEntities];

/*
  InitSpriteList is called when the program is first started.
  It just sets everything to zero and sets all pointers to NULL.
  It should never be called again.
*/

void initEntityList()
{
  int x;
  NumEntities = 0;
  memset(EntityList,0,sizeof(Entity) * MaxEntities);
  for(x = 0;x < MaxEntities;x++)EntityList[x].inuse = 0;
}

Entity *newEntity(Sprite *sprite)
{
  /*makesure we have the room for a new entity*/
  if(NumEntities + 1 >= MaxEntities)
  {
        fprintf(stderr, "Maximum Entities Reached.\n");
        exit(1);
  }
  
  int i = 0;
  /*Add the entity to the list*/
  for(i = 0; i < NumEntities; i++)
  {
    if(EntityList[i].inuse==0)
    {
      EntityList[i].inuse++;
      break;
    }
  }
  
  EntityList[i]->sprite = &sprite;
  
  return EntityList[i];
}

void drawEntityList(){
  int i = 0;
  for(i = 0; i < NumEntities; i++)
  {
    if(EntityList[i].inuse==1) 
      DrawSprite(EntityList[i]->sprite, EntityList[i]->sprite->image, EntityList[i].position.x, EntityList[i].position.y, 0);
  }
}


