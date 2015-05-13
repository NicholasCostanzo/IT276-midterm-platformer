#ifndef __LEVEL__
#define __LEVEL__

#include "sprite.h"

#define LEVELNAMELEN 80


typedef struct
{
  int loaded; /**<true if there is a level loaded*/
  char filename[LEVELNAMELEN]; /**<the filename and path of the level loaded*/
  Sprite *bgImage;  /**<the actual background image for the level*/
  Sprite *tileSet;  /**<the tile sheet for drawing tiles*/
  Sprite *enemy1;
  Sprite *enemy1alt1;
  Sprite *enemy2;
  Sprite *enemy2alt1;
  Sprite *enemy3;
  Sprite *enemy3alt1;
  Sprite *enemy9;
  Sprite *enemy9alt1;
  Sprite *enemy9alt2;
  Sprite *tileGoal; /**<tile for the level goal*/
  SDL_Rect bounds;  /**<absolute bounds for entities in this level*/
  int *tileMap; /**<allocated tilemap data*/
  int  tileWidth; /**<width of tiles for tile data*/
  int  tileHeight;/**< height of tile for tile data*/
}Level;


/**
 * @brief loads a game level into memory and sets it up for drawing
 * 
 * @param filename the path and filename of the level to load
 */
void LoadLevel(char *filename);

/*
 * @brief takes a level number and returns a string in the format "levels/level%i" so it may then be passed to LoadLevel
 */
char* numToFileName(int num);

/**
 * @brief Creates entities from the level file to reduce draw redundency
 */
void CreateLevelEntities();

/**
 * @brief Draws the level currently loaded into memory
 */
void DrawLevel();

void DrawPlayer();

/**
 * @brief unloads the level currently in memory
 */
void CloseLevel();

/**
 * @brief sets up level system to a clean state
 */
void InitLevelSystem();
#endif
