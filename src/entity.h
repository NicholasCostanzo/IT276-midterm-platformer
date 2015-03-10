#ifndef __ENTITY__
#define __ENTITY__
#include "sprite.h"

typedef struct entity_s
{
    int inuse;
    Sprite *sprite;
    int frame;
    SDL_Rect bbox; /**<bounding box for the entity - also holds position*/
    int velx;		/*<velocity in the x direction*/
    int vely;		/*<velocity in the y direction*/
    int health;		/*<health points, alive > 0*/
    int ST_JUMP;	/*<jump state*/
    float radius;
    void (*think)(struct entity_s *self);/**<pointer to the think function for this entity*/
    void (*die)(struct entity_s *self);/**<pointer to the function called when the entity dies*/
    void (*touch)(struct entity_s *self,struct entity_s *other);/**<pointer to the function called when the entity touches another entity*/
    /*void (*world_touch)(struct entity_s *self,vec2_t contact_point);*//**<function to call when an entity touches the world*/
    struct entity_s * target; /**<Pointer to the target entity*/
    struct entity_s * owner; /**<Pointer to the target entity*/
}Entity;

/**
 * @brief initializes the entity list so its ready to be used
 */
void initEntityList();

/**
 * @brief clears the data for all entities in the closeEntityList
 */
void closeEntityList();

/**
 * @brief draws all the entities in the EntityList
 */
void drawEntityList();

/**
 * @brief draws any blocks that make up a level
 */
void drawBlockEntityList();

/**
 * @brief draws an entity to the screen
 */
void drawEntity(Entity *ent, SDL_Surface *surface, int frame);


void updateEntityList();

/**
 * @brief tells all entities that think to do so
 */
void thinkEntityList();

/**
 * @brief creates a new entity
 */
Entity *newEntity();

/**
 * @brief creates a new block entity - blocks are world data
 */
Entity *newBlockEntity(Sprite *spr, int x, int y, int w, int h, int frame);

/**
 * @brief checks if box1 overlaps box2
 */
int collide(SDL_Rect box1,SDL_Rect box2);

/**
 * @brief check if box1 is standing on box2 with a variable distance between the two
 */
int onTop(SDL_Rect box1, SDL_Rect box2, int tolerance);


/**
 * @brief check if box1 is standing on a level block
 */
int isGrounded(SDL_Rect box1);

/**
 * @brief goes through all active block entities and checks if box1 collides with any of them
 */
int worldCollide(SDL_Rect box1);

/**
 * @brief sets up the player entity
 */
Entity *makePlayer();

/**
 * @brief figures out what the player should be doing every frame
 */
void playerThink(Entity *self);

/**
 * @brief clears an entity
 */
void freeEntity(Entity *ent);

void clearEntities();


#endif /*include guards*/
