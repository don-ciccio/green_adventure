#include "renderer.h"
#include "enemy.h"
#include "player.h"
#include "scene.h"

void renderer_draw_game(game_context *gc) {
  BeginMode3D(gc->camera);

  // Draw scene (including door)
  SceneDrawConfig config = {.camera = gc->camera,
                            .transform = MatrixIdentity(),
                            .layerMask = 0xFFFFFFFF,
                            .sortMode = SCENE_DRAW_SORT_FRONT_TO_BACK,
                            .drawBoundingBoxes = 0,
                            .drawCameraFrustum = 0};

  SceneDrawStats stats = DrawScene(gc->sceneId, config);

  // Remove any door-related debug drawing code, such as:
  // - Red debug cube for scene-managed door
  // - Blue debug cube for directly drawn door
  // - Any DrawModel calls for gc->doorModel

  // Draw player
  player_draw(&gc->player);

  // Draw enemies
  enemies_draw(gc);

  // Draw ground plane
  DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, WHITE);

  EndMode3D();

  // Draw HUD
  if (gc->paused) {
    DrawText("PAUSED", GetScreenWidth() / 2 - 50, GetScreenHeight() / 2, 20,
             RED);
  }

  // Draw scene stats (optional)
  DrawText(TextFormat("Meshes: %lu, Triangles: %lu", stats.meshDrawCount,
                      stats.trianglesDrawCount),
           10, 10, 20, WHITE);
}