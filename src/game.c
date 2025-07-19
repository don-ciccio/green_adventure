#include "game.h"
#include "player.h"
#include "enemy.h"
#include "camera.h"
#include "scene.h"
#include <math.h>

#define TREE_PATCH_COUNT 8

SceneNodeId CreateTreePatch(SceneId sceneId, SceneModelId modelId, int count) 
{
    SceneNodeId root = AcquireSceneNode(sceneId);
    
    for (int i = 0; i < count; i++) 
    {
        SceneNodeId firTreeNodeId = AcquireSceneNode(sceneId);
        SetSceneNodeModel(firTreeNodeId, modelId);
        // Medium spread for balanced density
        float rx = GetRandomValue(-800, 800) * 0.01f;  // -8 to +8 units
        float rz = GetRandomValue(-800, 800) * 0.01f;  // -8 to +8 units
        SetSceneNodePosition(firTreeNodeId, rx, 0, rz);
        
        // Bigger trees - more realistic forest size
        float scale = GetRandomValue(180, 220) * 0.01f;  // 1.8 to 2.2 scale (significantly taller than player)
        float scaleHeight = GetRandomValue(-15, 15) * 0.01f + scale;  // Slight height variation
        SetSceneNodeScale(firTreeNodeId, scale, scaleHeight, scale);  // Uniform X/Z scaling
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
    
    // Load fir tree model and create tree patches
    Model firTree = LoadModel("./assets/firtree-1.glb");
    if (firTree.meshCount > 0) {
        // Set texture filter for better quality
        if (firTree.materialCount > 0) {
            SetTextureFilter(firTree.materials[0].maps[MATERIAL_MAP_ALBEDO].texture, TEXTURE_FILTER_BILINEAR);
        }
        
        SceneModelId firTreeId = AddModelToScene(gc->sceneId, firTree, "fir tree", 1);
        
        // Balanced tree distribution
        for (int i = 0; i < TREE_PATCH_COUNT; i++) {
            SceneNodeId treePatchNodeId = CreateTreePatch(gc->sceneId, firTreeId, 20);  // 20 trees per patch
            SetSceneNodePosition(treePatchNodeId, (i % 4) * 25.0f - 37.5f, 0, (i / 4) * 25.0f);  // 25 unit spacing
            SetSceneNodeName(treePatchNodeId, TextFormat("TreePatch_%d", i));
        }
        
        TraceLog(LOG_INFO, "Created %d tree patches with fir trees", TREE_PATCH_COUNT);
    } else {
        TraceLog(LOG_ERROR, "Failed to load firtree-1.glb model!");
    }
    
    // Initialize camera
    camera_init(gc);
    
    // Initialize player
    player_init(&gc->player);
    player_load_model(&gc->player, "./assets/cesium_man.m3d");
    
    // Initialize enemies
    enemies_init(gc);
}

void game_handle_input(game_context *gc) {
    gc->running = !WindowShouldClose();
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        gc->paused = !gc->paused;
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
    player_cleanup(&gc->player);
    UnloadScene(gc->sceneId);
}