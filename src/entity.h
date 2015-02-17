#ifndef __ENTITY__
#define __ENTITY__

#include "sprite.h"

typedef struct entity_s
{
  int inuse;
  Sprite *sprite;
  int frame;
  int xpos;
  int ypos;
  struct entity_s * target;
  struct entity_s * owner;
} Entity;

void InitEntityList();
void CloseEntityList();
void DrawEntityList();
void UpdateEntityList();
void ThinkEntityList();

void freeEntity();
Entity *NewEntity(Sprite *sprite, int xpos, int ypos);

#endif
