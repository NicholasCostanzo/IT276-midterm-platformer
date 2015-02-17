#ifndef __ENTITY__
#define __ENTITY__

typedef struct entity_s
{
  int inuse;
  Sprite *sprite;
  int frame;
  vec2_t position;
  vec2_t velocity;
  SDL_Rect bbox;
  float radius;
  void (*think)(struct entity_s *self);
  void (*die)(struct entity_s *self);
  void (*touch)(struct entity_s *self, struct entity_s *other);
  void (*world_touch)(struct entity_s *self);
  struct entity_s * target;
  struct entity_s * owner;
} Entity;

void initEntityList();
void closeEntityList();
void drawEntityList();
void updateEntityList();
void thinkEntityList();
Entity *newEntity(Sprite *sprite);

#endif
