#ifndef CAMERA_H
#define CAMERA_H

#include "game_types.h"

// Camera function declarations
void camera_init(game_context *gc);
void camera_update(game_context *gc);
void camera_set_target(game_context *gc, Vector3 target);

#endif // CAMERA_H