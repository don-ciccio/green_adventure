#include "game.h"
#include "camera.h"
#include "enemy.h"
#include "lighting.h"
#include "player.h"
#include "scene.h"
#include <math.h>

#define TREE_PATCH_COUNT 4

SceneNodeId CreateTreePatch(SceneId sceneId, SceneModelId *modelIds,
                            int modelCount, int count) {
  SceneNodeId root = AcquireSceneNode(sceneId);

  for (int i = 0; i < count; i++) {
    SceneNodeId firTreeNodeId = AcquireSceneNode(sceneId);
    // Cycle through different tree models for variety
    SceneModelId currentModelId = modelIds[i % modelCount];
    SetSceneNodeModel(firTreeNodeId, currentModelId);
    // Medium spread for balanced density
    float rx = GetRandomValue(-800, 800) * 0.01f; // -8 to +8 units
    float rz = GetRandomValue(-800, 800) * 0.01f; // -8 to +8 units
    SetSceneNodePosition(firTreeNodeId, rx, 0, rz);

    // Smaller trees - reduced scale
    float scale = GetRandomValue(120, 160) *
                  0.01f; // 1.2 to 1.6 scale (smaller than before)
    float scaleHeight =
        GetRandomValue(-10, 10) * 0.01f + scale; // Slight height variation
    SetSceneNodeScale(firTreeNodeId, scale, scaleHeight,
                      scale); // Uniform X/Z scaling
    SetSceneNodeRotation(firTreeNodeId, 0, GetRandomValue(0, 360), 0);
    SetSceneNodeParent(firTreeNodeId, root);
  }

  return root;
}

void game_init(game_context *gc) {
  // Initialize game state
  gc->paused = false;
  gc->running = true;

  // Initialize scene
  gc->sceneId = LoadScene();

  // Load custom tree texture
  Texture2D treeTexture = LoadTexture("./assets/Colorsheet Tree Cold.png");
  SetTextureFilter(treeTexture, TEXTURE_FILTER_BILINEAR);

  // Load tree pack model and extract individual meshes
  Model treePack = LoadModel("./assets/LowPolyTreePack.glb");
  if (treePack.meshCount > 0) {
    TraceLog(LOG_INFO, "Loaded LowPolyTreePack with %d meshes",
             treePack.meshCount);

    // Create individual models for each mesh
    SceneModelId *treeModelIds =
        (SceneModelId *)malloc(treePack.meshCount * sizeof(SceneModelId));

    for (int i = 0; i < treePack.meshCount; i++) {
      // Create a model with only one mesh
      Model singleTreeModel = {0};
      singleTreeModel.meshCount = 1;
      singleTreeModel.meshes = &treePack.meshes[i];
      singleTreeModel.materialCount = 1;

      // Create a new material with the custom texture
      Material *customMaterial = (Material *)malloc(sizeof(Material));
      *customMaterial = LoadMaterialDefault();
      customMaterial->maps[MATERIAL_MAP_ALBEDO].texture = treeTexture;

      singleTreeModel.materials = customMaterial;

      // Set mesh material index
      int *meshMaterial = (int *)malloc(sizeof(int));
      *meshMaterial = 0;
      singleTreeModel.meshMaterial = meshMaterial;

      treeModelIds[i] = AddModelToScene(gc->sceneId, singleTreeModel,
                                        TextFormat("tree_mesh_%d", i), 1);
    }

    // Balanced tree distribution with variety
    for (int i = 0; i < TREE_PATCH_COUNT; i++) {
      SceneNodeId treePatchNodeId =
          CreateTreePatch(gc->sceneId, treeModelIds, treePack.meshCount,
                          12); // Reduced from 20 to 12 trees per patch
      SetSceneNodePosition(treePatchNodeId, (i % 2) * 30.0f - 15.0f, 0,
                           (i / 2) * 30.0f - 15.0f); // Adjusted spacing
      SetSceneNodeName(treePatchNodeId, TextFormat("TreePatch_%d", i));
    }

    TraceLog(LOG_INFO,
             "Created %d tree patches with %d different tree types using "
             "custom texture",
             TREE_PATCH_COUNT, treePack.meshCount);

    free(treeModelIds);
  } else {
    TraceLog(LOG_ERROR, "Failed to load LowPolyTreePack.glb model!");
  }

  // Initialize camera
  camera_init(gc);

  // Initialize player
  player_init(&gc->player);
  player_load_model(&gc->player, "./assets/greenman.glb"); // Changed from orc_warrior.glb

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
  
  // Accessory toggle controls
  if (IsKeyPressed(KEY_ONE)) {
    gc->player.showEquip[BONE_SOCKET_HAT] = !gc->player.showEquip[BONE_SOCKET_HAT];
  }
  if (IsKeyPressed(KEY_TWO)) {
    gc->player.showEquip[BONE_SOCKET_HAND_R] = !gc->player.showEquip[BONE_SOCKET_HAND_R];
  }
  if (IsKeyPressed(KEY_THREE)) {
    gc->player.showEquip[BONE_SOCKET_HAND_L] = !gc->player.showEquip[BONE_SOCKET_HAND_L];
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
  UnloadScene(gc->sceneId);
}