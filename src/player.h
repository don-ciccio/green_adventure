#ifndef PLAYER_H
#define PLAYER_H

#include "game_types.h"

// Player function declarations
void player_init(player_t *player);
void player_load_model(player_t *player, const char *model_path);
void player_update(game_context *gc);
void player_draw(const player_t *player, Shader lightingShader);
void player_cleanup(player_t *player);
BoundingBox player_get_bbox(const player_t *player);
void player_handle_input(game_context *gc, Vector3 *movement, bool *moved);
void player_handle_animation(player_t *player, bool moved);
void player_handle_collision(game_context *gc, Vector3 old_position);

#endif // PLAYER_H