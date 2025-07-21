#include "renderer.h"
#include "collision.h"
#include "enemy.h"
#include "lighting.h"
#include "player.h"
#include "scene.h"

void renderer_draw_game(game_context *gc) {
  // Update camera position in shader
  float cameraPos[3] = {gc->camera.position.x, gc->camera.position.y,
                        gc->camera.position.z};
  SetShaderValue(gc->lightingShader,
                 gc->lightingShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                 SHADER_UNIFORM_VEC3);

  // Update light values
  for (int i = 0; i < gc->lightCount; i++) {
    UpdateLightValues(gc->lightingShader, gc->lights[i], i);
  }

  BeginMode3D(gc->camera);

  // Draw scene with lighting shader
  SceneDrawConfig config = {.camera = gc->camera,
                            .transform = MatrixIdentity(),
                            .layerMask = 0xFFFFFFFF,
                            .sortMode = SCENE_DRAW_SORT_FRONT_TO_BACK,
                            .drawBoundingBoxes = 0,
                            .drawCameraFrustum = 0,
                            .shader = gc->lightingShader};

  SceneDrawStats stats = DrawScene(gc->sceneId, config);

  // Draw player with lighting
  player_draw(&gc->player, gc->lightingShader);

  // Draw enemies
  enemies_draw(gc);

  // Debug: Draw collision bounding boxes
  collision_debug_draw(&gc->collisionSystem);

  // Draw ground plane
  DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, WHITE);

  // Draw grid on the floor
  DrawGrid(60, 1.0f);

  // Draw light positions for debugging
  for (int i = 0; i < gc->lightCount; i++) {
    if (gc->lights[i].enabled && gc->lights[i].type == LIGHT_POINT) {
      DrawSphere(gc->lights[i].position, 0.2f, gc->lights[i].color);
      // Draw light info text - Fixed: GetWorldToScreen returns Vector2
      Vector2 screenPos = GetWorldToScreen(gc->lights[i].position, gc->camera);
      DrawText(TextFormat("Light %d", i), (int)screenPos.x,
               (int)screenPos.y - 20, 16, WHITE);
    }
  }

  EndMode3D();

  // Draw HUD with lighting debug info
  if (gc->paused) {
    DrawText("PAUSED", GetScreenWidth() / 2 - 50, GetScreenHeight() / 2, 20,
             RED);
  }

  // Enhanced debug info
  DrawText(TextFormat("Meshes: %lu, Triangles: %lu", stats.meshDrawCount,
                      stats.trianglesDrawCount),
           10, 10, 20, WHITE);
  DrawText("Press Y/R/G/B to toggle lights", 10, 30, 20, WHITE);
  DrawText("Press C to toggle collision debug", 10, 50, 20, WHITE);
  DrawText(TextFormat("Shader ID: %d", gc->lightingShader.id), 10, 70, 20,
           WHITE);
  DrawText(TextFormat("Active Lights: %d", gc->lightCount), 10, 90, 20, WHITE);
  DrawText(TextFormat("Collision Debug: %s", collision_is_debug_enabled() ? "ON" : "OFF"), 10, 110, 20, WHITE);

  // Show individual light status
  for (int i = 0; i < gc->lightCount; i++) {
    Color statusColor = gc->lights[i].enabled ? GREEN : RED;
    DrawText(
        TextFormat("Light %d: %s", i, gc->lights[i].enabled ? "ON" : "OFF"), 10,
        130 + i * 20, 16, statusColor);
  }
}