#ifndef ENEMY_H
#define ENEMY_H

#include "game_types.h"

// Enemy function declarations
void enemies_init(game_context *gc);
void enemies_update(game_context *gc);
void enemies_draw(const game_context *gc);
BoundingBox enemy_get_bbox(const enemy_t *enemy);
void enemy_init_single(enemy_t *enemy, Vector3 position);

#endif // ENEMY_H