#ifndef GAME_H
#define GAME_H

#include "game_types.h"

// Game function declarations
void game_init(game_context *gc);
void game_update(game_context *gc);
void game_cleanup(game_context *gc);
void game_handle_input(game_context *gc);

#endif // GAME_H