#ifndef RENDERER_H
#define RENDERER_H

#include "game_types.h"

// Renderer function declarations
void renderer_draw_game(game_context *gc);  // Remove const
void renderer_draw_hud(const game_context *gc);
void renderer_draw_3d_scene(const game_context *gc);

#endif // RENDERER_H