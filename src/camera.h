#ifndef CAMERA_H
#define CAMERA_H

#include "game_types.h"

void camera_init(game_context *gc);
void camera_update(game_context *gc);
void camera_set_target(game_context *gc, Vector3 target);

// New camera mode functions
void camera_set_mode(game_context *gc, GameCameraMode mode);
void camera_update_orthographic(game_context *gc);
void camera_update_third_person(game_context *gc);
bool camera_is_indoor_position(game_context *gc, Vector3 position);

#endif // CAMERA_H