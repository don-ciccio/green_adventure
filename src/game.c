#include "game.h"
#include "camera.h"
#include "collision.h"
#include "enemy.h"
#include "lighting.h"
#include "player.h"
#include "scene.h"
#include <math.h>

// Add function declaration
void assign_house_textures(Model* houseModel);
void debug_model_info(Model* model);

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
    
    // Assign textures to the house model
    assign_house_textures(&houseModel);
    
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

void auto_assign_textures(Model* houseModel) {
    // List all texture files in the directory
    FilePathList textureFiles = LoadDirectoryFiles("./assets/Textures/");
    
    for (int i = 0; i < houseModel->meshCount; i++) {
        // Try to find a matching texture file
        // This assumes your mesh materials have names that match texture files
        char expectedTextureName[256];
        snprintf(expectedTextureName, sizeof(expectedTextureName), 
                "mesh_%d", i); // Adjust naming convention as needed
        
        // In any function where you use textureFiles.count, change:
        // for (int j = 0; j < textureFiles.count; j++) {
        // to:
        for (unsigned int j = 0; j < textureFiles.count; j++) {
            const char* fileName = GetFileName(textureFiles.paths[j]);
            
            // Check if filename matches (implement your matching logic)
            if (strstr(fileName, expectedTextureName) != NULL) {
                Texture2D texture = LoadTexture(textureFiles.paths[j]);
                houseModel->materials[houseModel->meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                TraceLog(LOG_INFO, "Assigned texture %s to mesh %d", fileName, i);
                break;
            }
        }
    }
    
    UnloadDirectoryFiles(textureFiles);
}

void cleanup_house_textures(Model* houseModel) {
    for (int i = 0; i < houseModel->materialCount; i++) {
        if (houseModel->materials[i].maps[MATERIAL_MAP_DIFFUSE].texture.id > 0) {
            UnloadTexture(houseModel->materials[i].maps[MATERIAL_MAP_DIFFUSE].texture);
        }
    }
}

void debug_model_info(Model* model) {
    TraceLog(LOG_INFO, ">>> Model has %d meshes and %d materials", 
             model->meshCount, model->materialCount);
    
    for (int i = 0; i < model->meshCount; i++) {
        TraceLog(LOG_INFO, ">>> Mesh %d uses material %d", i, model->meshMaterial[i]);
    }
}

void assign_house_textures(Model* houseModel) {
    // First, debug the model structure
    TraceLog(LOG_INFO, "========== TEXTURE ASSIGNMENT START ==========");
    debug_model_info(houseModel);
    
    // Load some basic textures from your texture folder
    Texture2D wallTexture = LoadTexture("./assets/Textures/PlasterWhite.jpg");
    Texture2D woodTexture = LoadTexture("./assets/Textures/Wood.jpg");
    Texture2D windowTexture = LoadTexture("./assets/Textures/Windows.jpg");
    Texture2D doorTexture = LoadTexture("./assets/Textures/Door.jpg");
    
    // Check if textures loaded successfully and use a proper default texture
    Texture2D defaultTexture = { 0 }; // Initialize with zero
    defaultTexture.id = 1; // Use texture ID 1 (white texture)
    defaultTexture.width = 1;
    defaultTexture.height = 1;
    defaultTexture.mipmaps = 1;
    defaultTexture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    
    TraceLog(LOG_INFO, ">>> TEXTURE LOADING RESULTS:");
    if (wallTexture.id == 0) {
        TraceLog(LOG_WARNING, ">>> Failed to load wall texture, using default");
        wallTexture = defaultTexture;
    } else {
        TraceLog(LOG_INFO, ">>> Successfully loaded wall texture (ID: %d)", wallTexture.id);
    }
    if (woodTexture.id == 0) {
        TraceLog(LOG_WARNING, ">>> Failed to load wood texture, using default");
        woodTexture = defaultTexture;
    } else {
        TraceLog(LOG_INFO, ">>> Successfully loaded wood texture (ID: %d)", woodTexture.id);
    }
    if (windowTexture.id == 0) {
        TraceLog(LOG_WARNING, ">>> Failed to load window texture, using default");
        windowTexture = defaultTexture;
    } else {
        TraceLog(LOG_INFO, ">>> Successfully loaded window texture (ID: %d)", windowTexture.id);
    }
    if (doorTexture.id == 0) {
        TraceLog(LOG_WARNING, ">>> Failed to load door texture, using default");
        doorTexture = defaultTexture;
    } else {
        TraceLog(LOG_INFO, ">>> Successfully loaded door texture (ID: %d)", doorTexture.id);
    }
    
    // Assign textures to meshes
    TraceLog(LOG_INFO, ">>> MESH TEXTURE ASSIGNMENT:");
    for (int i = 0; i < houseModel->meshCount; i++) {
        int materialIndex = houseModel->meshMaterial[i];
        
        // Assign different textures based on mesh index
        switch(i % 4) {
            case 0:
                houseModel->materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].texture = wallTexture;
                TraceLog(LOG_INFO, ">>> Assigned WALL texture to mesh %d (material %d)", i, materialIndex);
                break;
            case 1:
                houseModel->materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].texture = woodTexture;
                TraceLog(LOG_INFO, ">>> Assigned WOOD texture to mesh %d (material %d)", i, materialIndex);
                break;
            case 2:
                houseModel->materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].texture = windowTexture;
                TraceLog(LOG_INFO, ">>> Assigned WINDOW texture to mesh %d (material %d)", i, materialIndex);
                break;
            case 3:
                houseModel->materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].texture = doorTexture;
                TraceLog(LOG_INFO, ">>> Assigned DOOR texture to mesh %d (material %d)", i, materialIndex);
                break;
        }
    }
    TraceLog(LOG_INFO, "========== TEXTURE ASSIGNMENT COMPLETE ==========");
}