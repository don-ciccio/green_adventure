#include "game.h"
#include "camera.h"
#include "collision.h"
#include "enemy.h"
#include "lighting.h"
#include "player.h"
#include "scene.h"
#include <math.h>

void game_init(game_context *gc) {
  // Initialize game state
  gc->paused = false;
  gc->running = true;

  // Initialize scene
  gc->sceneId = LoadScene();

  // Initialize collision system
  collision_init(&gc->collisionSystem);

  // Load and place a single house model
  Model houseModel = LoadModel("./assets/house.glb");
  if (houseModel.meshCount > 0) {
    TraceLog(LOG_INFO, "Loaded house.glb with %d meshes", houseModel.meshCount);

    // Add house model to scene
    SceneModelId houseModelId =
        AddModelToScene(gc->sceneId, houseModel, "house_model", 1);

    // Create a single house node
    SceneNodeId houseNodeId = AcquireSceneNode(gc->sceneId);
    SetSceneNodeModel(houseNodeId, houseModelId);

    // Position the house at a reasonable distance from spawn
    SetSceneNodePosition(houseNodeId, 10.0f, 0.0f, 10.0f);

    // Keep normal scale and no rotation for clarity
    SetSceneNodeScale(houseNodeId, 1.0f, 1.0f, 1.0f);
    SetSceneNodeRotation(houseNodeId, 0.0f, 0.0f, 0.0f);
    SetSceneNodeName(houseNodeId, "MainHouse");

    TraceLog(LOG_INFO, "Created simple house scene");
  } else {
    TraceLog(LOG_ERROR, "Failed to load house.glb model!");
  }

  // Initialize camera
  camera_init(gc);

  // Initialize player
  player_init(&gc->player);
  player_load_model(&gc->player, "./assets/greenman.glb");

  // Initialize enemies
  enemies_init(gc);

  // Initialize lighting system
  lighting_init(gc);
}

void game_handle_input(game_context *gc) {
  gc->running = !WindowShouldClose();

  if (IsKeyPressed(KEY_ESCAPE)) {
    gc->paused = !gc->paused;
  }

  // Light toggle controls
  if (IsKeyPressed(KEY_Y)) {
    gc->lights[0].enabled = !gc->lights[0].enabled;
  }
  if (IsKeyPressed(KEY_R)) {
    gc->lights[1].enabled = !gc->lights[1].enabled;
  }
  if (IsKeyPressed(KEY_G)) {
    gc->lights[2].enabled = !gc->lights[2].enabled;
  }

  // Collision debug toggle
  if (IsKeyPressed(KEY_C)) {
    collision_toggle_debug();
  }

  // Accessory toggle controls
  if (IsKeyPressed(KEY_ONE)) {
    gc->player.showEquip[BONE_SOCKET_HAT] =
        !gc->player.showEquip[BONE_SOCKET_HAT];
  }
  if (IsKeyPressed(KEY_TWO)) {
    gc->player.showEquip[BONE_SOCKET_HAND_R] =
        !gc->player.showEquip[BONE_SOCKET_HAND_R];
  }
  if (IsKeyPressed(KEY_THREE)) {
    gc->player.showEquip[BONE_SOCKET_HAND_L] =
        !gc->player.showEquip[BONE_SOCKET_HAND_L];
  }
}

void game_update(game_context *gc) {
  game_handle_input(gc);

  if (!gc->paused) {
    player_update(gc);
    camera_update(gc);
    enemies_update(gc);
  }
}

void game_cleanup(game_context *gc) {
  lighting_cleanup(gc);
  player_cleanup(&gc->player);
  collision_cleanup(&gc->collisionSystem);
  UnloadScene(gc->sceneId);
}

// In your game drawing/rendering function, add:
void game_draw(game_context *gc) {
  // ... existing drawing code ...

  // Debug: Draw collision bounding boxes
  collision_debug_draw(&gc->collisionSystem);

  // ... rest of drawing code ...
}