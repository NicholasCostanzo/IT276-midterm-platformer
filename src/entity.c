#include "entity.h"
#include "level.h"
#include "levelTree.h"

#define __MaxEntities 4096

extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Surface *screen;
extern SDL_Rect Camera;
extern struct node *levelMap;

Entity * __entityList = NULL;
Entity EntityList[__MaxEntities];
Entity BlockEntityList[__MaxEntities];
Entity *player = NULL;


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

void thinkEntityList()
{
  int i;
  for(i = 0;i < __MaxEntities;i++)
  {
    if(EntityList[i].inuse)
    {
      if(EntityList[i].think != NULL)
      {
        EntityList[i].think(&EntityList[i]);
      }
    }
  }
}

void drawEntityList()
{
  int i;
  for(i = 0; i < NumEntities; i++)
  {
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
  if(ent->inuse==1 && ent->sprite) DrawSprite(ent->sprite, surface, ent->bbox.x - Camera.x, ent->bbox.y - Camera.y, frame);
}


Entity *newEntity()
{
  int i;
    for (i = 0; i < __MaxEntities; i++)
    {
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
      NumEntities++;
      return &BlockEntityList[i];
    }
  }
    return NULL;
}

int collide(SDL_Rect box1,SDL_Rect box2)
{
  /*check to see if box 1 and box 2 clip, then check to see if box1 is in box or vice versa*/
  if((box1.x + box1.w > box2.x) && (box1.x < box2.x+box2.w) && (box1.y + box1.h > box2.y) && (box1.y < box2.y+box2.h))
    return 1;
  return 0;
}

int onTop(SDL_Rect box1, SDL_Rect box2, int tolerance)
{
  /*check to see if box 1 is standing on top of box 2, with a variable tolerance (actual space between the two)*/
  if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x+box2.w) && (box1.y + box1.h <= box2.y + tolerance) && (box1.y + box2.h >= box2.y - tolerance))
    return 1;
  return 0;
}

int isGrounded(SDL_Rect box1)
{
  int i;
  SDL_Rect box2;
  for(i = 0; i < __MaxEntities; i++)
  {
    if(BlockEntityList[i].inuse == 1)
    {
      box2 = BlockEntityList[i].bbox;
      if(onTop(box1, box2, 10)) return 1;
    }
  }
  return 0;
}

int killCollide(SDL_Rect box1)
{
  int i;
  SDL_Rect box2;
  for (i = 0; i < __MaxEntities; i++)
  {
    if(EntityList[i].inuse == 1 && EntityList[i].canKill == 1)
    {
      box2 = EntityList[i].bbox;
      if(collide(box1, box2)) return 1;
    }
  }
  return 0;
}

int goalCollide(SDL_Rect box1)
{
  int i;
  SDL_Rect box2;
  for(i = 0; i < __MaxEntities; i++)
  {
    if(EntityList[i].inuse == 1 && EntityList[i].levelGoal >= 1)
    {
      box2 = EntityList[i].bbox;
      if(collide(box1, box2)) return EntityList[i].levelGoal;
    }
  }
  return 0;
}

int worldCollide(SDL_Rect box1) /*TODO: sort BlockEntityList by x and y, then replace this with a faster search (binary search or similar) to improve performance*/
{
  int i;
  SDL_Rect box2;
  for (i = 0; i < __MaxEntities; i++)
  {
    if(BlockEntityList[i].inuse == 1)
    {
      box2 = BlockEntityList[i].bbox;
      if(collide(box1, box2)) return 1;
    }
  }
  return 0;
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
  player->health = 100;
  player->frame = 0;
  player->ST_JUMP = 0;
  player->inuse = 1;
  player->think = playerThink;
  return player;
}

void playerThink(Entity *self)
{
  int i, numkeys, goalNum;
  SDL_Rect box = self->bbox;
  Uint8 *keys = SDL_GetKeyState(&numkeys);   
   
   if(self->health > 0)
   {
      if(self->velx)
      {
	box.x = self->bbox.x + self->velx;
	if(!worldCollide(box)) self->bbox.x = box.x;
      }
      if(self->vely)
      {
	box.y = self->bbox.y + self->vely;
	if(!worldCollide(box)) self->bbox.y = box.y;
      }
      
      if(keys[SDLK_d]) /*<move the player right*/
      {
	  if(self->velx < 10)self->velx += 2;
      }
      else if (self->velx > 0) self->velx -= 2;
      
      if(keys[SDLK_a]) /*<move the player left*/
      {
	  if(self->velx > -10)self->velx -= 2;
      }
      else if (self->velx < 0) self->velx += 2;
      
      if(keys[SDLK_SPACE] && isGrounded(box))
      {
	  self->ST_JUMP = 1;
      }
      
      if(self->ST_JUMP)
      {
	if(self->vely > -10) self->vely -= 2;
	else self->ST_JUMP = 0;
      }
      else{
	  if(self->vely < 10) self->vely += 2;
      }
      
      goalNum = goalCollide(self->bbox);
      if(goalNum && levelMap && levelMap->child)
      {
	levelMap = levelMap->child;
	for(i = 0; i < goalNum-1; i++)
	{
	  if(levelMap->sibling)levelMap = levelMap->sibling;
	}
	for(i = 0; i < 2; i++){ /*something breaks graphics here unless you do all of it twice for some reason. this is just a hacky quick-fix TODO: fucking fix it*/
	CloseLevel();
	CloseSprites();
	clearEntities();
	
	Init_Graphics();
	initEntityList();
	InitLevelSystem();
	LoadLevel(numToFileName(levelMap->level)); 
	CreateLevelEntities();
	player = makePlayer();
	}
	
	fprintf(stdout, "level: %d \n", levelMap->level);
      }
      
      if(killCollide(self->bbox))
      {
	self->health = 0;
      }
   }
   else
   {
     self->health = 100;
     player->bbox.x = 100;
     player->bbox.y = 672;
  }
}

void freeEntity(Entity *ent)
{
    ent->inuse=0;
    NumEntities--;
    if(ent->sprite != NULL) FreeSprite(ent->sprite);
    /*handle freeing resources like Sprite data*/
    memset(ent,0,sizeof(Entity));
}

void clearEntities()
{
  int i = 0;
  for(i = 0;i < __MaxEntities;i++)
  {
    freeEntity(&EntityList[i]);
    freeEntity(&BlockEntityList[i]);
  }
}



/*eol@eof*/
