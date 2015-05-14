#include "entity.h"
#include "level.h"
#include "levelTree.h"

#define __MaxEntities 4096

extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Surface *screen;
extern SDL_Rect Camera;
extern struct node *levelMap;
extern int done;

Entity * __entityList = NULL;
Entity EntityList[__MaxEntities];
Entity BlockEntityList[__MaxEntities];
Entity *player = NULL;

/**
 * TODO: split some of this code of into player.c, enemy.c, etc.
 */

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
  if((box1.x + box1.w > box2.x) && (box1.x < box2.x+box2.w) && (box1.y + box1.h <= box2.y + tolerance) && (box1.y + box2.h >= box2.y - tolerance))
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
      if(onTop(box1, box2, 2)) return 1;
    }
  }
  return 0;
}

int enemyBounce(SDL_Rect box1)
{
  int i;
  SDL_Rect box2;
  for(i = 0; i < __MaxEntities; i++)
  {
    if(EntityList[i].inuse && EntityList[i].isBad)
    {
      
      box2 = EntityList[i].bbox;
      if(onTop(box1, box2, 10)){
	EntityList[i].health --; /*TODO: also make temporarily invinsible*/
	return 1;
      }
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

int killCollideNonPlayer(SDL_Rect box1)
{
  int i;
  SDL_Rect box2;
  for (i = 0; i < __MaxEntities; i++)
  {
    if(!EntityList[i].isBad && EntityList[i].inuse && EntityList[i].canKill)
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
  int i, numkeys, goalNum, level;
  SDL_Rect box = self->bbox;
  Uint8 *keys = SDL_GetKeyState(&numkeys);   
   
   if(self->health > 0)
   {
      if(self->velx)
      {
	box.x = self->bbox.x + self->velx;
	if(!worldCollide(box)) self->bbox.x = box.x;
	box.x = self->bbox.x;
      }
      if(self->vely)
      {
	box.y = self->bbox.y + self->vely;
	if(!worldCollide(box)) self->bbox.y = box.y;
	else if(self->vely > 0)
	{
	  box.y = self->bbox.y + 1;
	  if(!worldCollide(box)) self->bbox.y = box.y;
	}
	box.y = self->bbox.y;
      }
      
      if(keys[SDLK_d] || keys[SDLK_RIGHT]) /*<move the player right*/
      {
	  if(self->velx < 10)self->velx += 2;
      }
      else if (self->velx > 0) self->velx -= 2;
      
      if(keys[SDLK_a] || keys[SDLK_LEFT]) /*<move the player left*/
      {
	  if(self->velx > -10)self->velx -= 2;
      }
      else if (self->velx < 0) self->velx += 2;
      
      if(keys[SDLK_SPACE] && isGrounded(box))
      {
	  self->ST_JUMP = 1;
	  self->vely = -20;
      }
      if(self->ST_JUMP)
      {
	if(self->vely >= 0) self->ST_JUMP = 0;
      }
      if(self->vely < 10) self->vely += 2;
      
      goalNum = goalCollide(self->bbox);
      if(goalNum && levelMap && levelMap->child)
      {
	levelMap = levelMap->child;
	for(i = 0; i < goalNum-1; i++)
	{
	  if(levelMap->sibling)levelMap = levelMap->sibling;
	}
	CloseLevel(); /*TODO: split these calls off to some sort of reload function, and consider splitting close and init calls out of main and into their own file*/
	CloseSprites();
	clearEntities();
	
	Init_Graphics();
	initEntityList();
	InitLevelSystem();
	level = levelMap->level;
	if (level == 0) level = 999;
	LoadLevel(numToFileName(level)); 
	CreateLevelEntities();
	player = makePlayer();
	
	fprintf(stdout, "level: %d \n", levelMap->level);
      }
      else if(goalNum && levelMap)
      {
	level=999;
	CloseLevel();
	CloseSprites();
	clearEntities();
	
	Init_Graphics();
	initEntityList();
	InitLevelSystem();
	LoadLevel(numToFileName(level)); 
	CreateLevelEntities();
	player = makePlayer();
	
	fprintf(stdout, "level: %d \n", levelMap->level);
      }
      
      if(enemyBounce(self->bbox))
      {
	self->ST_JUMP = 1;
	self->vely = -20;
      }
      else if(killCollide(self->bbox))
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

void enemy3Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
    if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy3alt1Think(Entity *self)
{
  int temp;
  int temp2;
  SDL_Rect box = self->bbox;
  SDL_Rect box2= self->bbox;
  
  if(self->velx >= 0) box2.x += 15; /*box 2 looks ahead of the direction the enmey is moving, and down to where it might fall*/
  if(self->velx <  0) box2.x -= 15;
  box2.y += 15;
  
  if(self->health > 0)
  {
    if(self->velx) /*move in current direction until a collision or pit is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      temp2=worldCollide(box2);
      if(temp2 && !temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy2Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
    if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(isGrounded(box))
    {
	self->ST_JUMP = 1;
    }
    if(self->ST_JUMP)
    {
      if(self->vely > -10) self->vely -= 2;
      else self->ST_JUMP = 0;
    }
    else if(self->vely < 10) self->vely += 2;
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy2alt1Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
    if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(isGrounded(box))
    {
	self->ST_JUMP = 1;
	self->vely = -20;
    }
    if(self->ST_JUMP)
    {
      if(self->vely >= 0) self->ST_JUMP = 0;
    }
    if(self->vely < 10) self->vely += 2;
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy1Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
   if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(self->ST_JUMP)
    {
      if(self->vely > -10) self->vely -= 2;
      else self->ST_JUMP = 0;
    }
    else{
      if(self->vely < 10) self->vely += 2;
      else self->ST_JUMP = 1;
    }
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy1alt1Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
   if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(self->ST_JUMP)
    {
      if(self->velx > -10) self->velx -= 2;
      else self->ST_JUMP = 0;
    }
    else{
      if(self->velx < 10) self->velx += 2;
      else self->ST_JUMP = 1;
    }
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
  }
}

void enemy9Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int temp;
  
  if(self->health > 0)
  {
   if(self->velx) /*move in current direction until a collision is detected, then change direction*/
    {
      box.x = box.x + self->velx;
      temp = worldCollide(box);
      if(!temp) self->bbox.x += self->velx;
      else self->velx *= -1;
    }
    if(self->vely)
    {
      box.y = self->bbox.y + self->vely;
      if(!worldCollide(box)) self->bbox.y = box.y;
    }
    
    if(self->ST_JUMP)
    {
      if(self->vely > -10) self->vely -= 2;
      else self->ST_JUMP = 0;
    }
    else{
      if(self->vely < 10) self->vely += 2;
      else self->ST_JUMP = 1;
    }
    
    if(killCollideNonPlayer(self->bbox))
    {
      self->health = 0;
    }
  }
  else
  {
    self->inuse = 0;
    done = 1;
    exit(0);
  }
}

void enemy9alt1Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  
  if(self->health > 0)
  {
    if(self->velx)
      {
	box.x = self->bbox.x + self->velx;
	if(!worldCollide(box)) self->bbox.x = box.x;
	else self->velx*=-1;
	box.x = self->bbox.x;
      }
      if(self->vely)
      {
	box.y = self->bbox.y + self->vely;
	if(!worldCollide(box)) self->bbox.y = box.y;
	else self->vely*=-1;
	box.y = self->bbox.y;
      }
      
      if(killCollideNonPlayer(self->bbox))
      {
	self->health = 0;
      }
  }
  else
  {
    self->inuse = 0;
    done = 1;
    exit(0);
  }
}

void enemy9alt2Think(Entity *self)
{
  SDL_Rect box = self->bbox;
  int rng = rand_int(2);
  
  if(self->health > 0)
  {
    if(self->velx)
      {
	box.x = self->bbox.x + self->velx;
	if(!worldCollide(box)) self->bbox.x = box.x;
	else{
	  self->vely=self->velx;
	  self->velx*=0;
	  if(rng == 1)self->vely*=-1;
	}
	box.x = self->bbox.x;
      }
      if(self->vely)
      {
	box.y = self->bbox.y + self->vely;
	if(!worldCollide(box)) self->bbox.y = box.y;
	else{
	  self->velx = self->vely;
	  self->vely*=0;
	  if(rng == 1)self->velx*=-1;
	}
	box.y = self->bbox.y;
      }
      
      if(killCollideNonPlayer(self->bbox))
      {
	self->health = 0;
      }
  }
  else
  {
    self->inuse = 0;
    done = 1;
    exit(0);
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
