#include "game.h"
#include "renderer.h"

int main() {
  InitWindow(WIDTH, HEIGHT, "Third Person Game");
  SetTargetFPS(60);

  game_context gc = {0};
  game_init(&gc); // This should be game_init, not load_game

  DisableCursor();

  while (gc.running) {
    game_update(&gc);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    renderer_draw_game(&gc);
    EndDrawing();
  }

  game_cleanup(&gc);
  CloseWindow();

  return 0;
}