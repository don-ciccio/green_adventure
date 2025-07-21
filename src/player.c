#include "player.h"
#include "collision.h"
#include "enemy.h"

void player_init(player_t *player) {
  player->position =
      (Vector3){10.0f, 1.0f, 10.0f}; // Spawn inside the house at (10, 1, 10)
  player->size = (Vector3){1.0f, 2.0f, 1.0f};
  player->color = WHITE;
  player->rotation_y = 0.0f;
  player->move_speed = PLAYER_MOVE_SPEED;
  player->animsCount = 0;
  player->animFrameCounter = 0;
  player->animId = 0;
  player->anims = NULL;

  // Initialize accessory system
  for (int i = 0; i < BONE_SOCKETS; i++) {
    player->showEquip[i] = true;
    player->boneSocketIndex[i] = -1;
  }
}

void player_load_model(player_t *player, const char *model_path) {
  player->model = LoadModel(model_path);
  player->anims = LoadModelAnimations(model_path, &player->animsCount);

  // Load accessory models
  player->equipModels[BONE_SOCKET_HAT] = LoadModel("./assets/greenman_hat.glb");
  player->equipModels[BONE_SOCKET_HAND_R] =
      LoadModel("./assets/greenman_sword.glb");
  player->equipModels[BONE_SOCKET_HAND_L] =
      LoadModel("./assets/greenman_shield.glb");

  // Find bone socket indices
  for (int i = 0; i < player->model.boneCount; i++) {
    if (TextIsEqual(player->model.bones[i].name, "socket_hat")) {
      player->boneSocketIndex[BONE_SOCKET_HAT] = i;
      continue;
    }

    if (TextIsEqual(player->model.bones[i].name, "socket_hand_R")) {
      player->boneSocketIndex[BONE_SOCKET_HAND_R] = i;
      continue;
    }

    if (TextIsEqual(player->model.bones[i].name, "socket_hand_L")) {
      player->boneSocketIndex[BONE_SOCKET_HAND_L] = i;
      continue;
    }
  }
}

BoundingBox player_get_bbox(const player_t *player) {
  return (BoundingBox){
      .min = (Vector3){player->position.x - player->size.x / 2,
                       player->position.y - player->size.y / 2,
                       player->position.z - player->size.z / 2},
      .max = (Vector3){player->position.x + player->size.x / 2,
                       player->position.y + player->size.y / 2,
                       player->position.z + player->size.z / 2}};
}

void player_handle_input(game_context *gc, Vector3 *movement, bool *moved) {
  // Fixed isometric movement directions that match visual expectations
  // For isometric view: adjust directions to feel natural from camera perspective
  
  // Analog input system for smooth movement
  float input_x = 0.0f;
  float input_z = 0.0f;

  // Keyboard input - adjusted directions for natural isometric feel
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
    input_z -= 1.0f; // Move towards camera (negative Z)
  }
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
    input_z += 1.0f; // Move away from camera (positive Z)
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    input_x += 1.0f; // Move right (positive X)
  }
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    input_x -= 1.0f; // Move left (negative X)
  }

  // Gamepad support
  if (IsGamepadAvailable(0)) {
    float gamepad_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float gamepad_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    
    // Direct mapping for gamepad (no transformation needed)
    input_x += gamepad_x;
    input_z -= gamepad_y; // Invert Y axis for forward/backward
  }

  // Check if there's any movement input
  if (input_x != 0.0f || input_z != 0.0f) {
    *moved = true;
    
    // Normalize movement for consistent speed
    float magnitude = sqrtf(input_x * input_x + input_z * input_z);
    if (magnitude > 0.0f) {
      input_x /= magnitude;
      input_z /= magnitude;
      
      movement->x = input_x * gc->player.move_speed;
      movement->z = input_z * gc->player.move_speed;
    }
  }
}

void player_handle_animation(player_t *player, bool moved) {
  if (player->animsCount > 0) {
    int targetAnimId = 1; // Default to idle animation

    // Determine which animation to play based on player state
    if (IsKeyDown(KEY_SPACE)) {
      targetAnimId = 3; // Attack animation
    } else if (moved) {
      targetAnimId = 2; // Move/running animation
    } else {
      targetAnimId = 1; // Idle animation
    }

    // Switch animation if needed
    if (player->animId != targetAnimId) {
      player->animId = targetAnimId;
      player->animFrameCounter = 0;
    }

    // Update animation frame
    player->animFrameCounter++;
    if (player->animFrameCounter >= player->anims[player->animId].frameCount) {
      player->animFrameCounter = 0;
    }

    // Apply the animation to the model
    UpdateModelAnimation(player->model, player->anims[player->animId],
                         player->animFrameCounter);

    // Keep the manual animation change with C key for testing
    if (IsKeyPressed(KEY_C)) {
      player->animFrameCounter = 0;
      player->animId++;
      if (player->animId >= player->animsCount) {
        player->animId = 0;
      }
      UpdateModelAnimation(player->model, player->anims[player->animId], 0);
    }
  }
}

void player_handle_collision(game_context *gc, Vector3 old_position) {
    gc->player.bbox = player_get_bbox(&gc->player);
    
    // Use mesh-based collision detection instead of bounding box
    float playerRadius = 0.3f; // Adjust based on your player size
    
    // Check if the new position is valid using raycasting
    bool canMove = collision_can_move_to_position(&gc->collisionSystem, old_position, gc->player.position, playerRadius);
    
    if (!canMove) {
        // Try sliding movement
        Vector3 movement = Vector3Subtract(gc->player.position, old_position);
        Vector3 slideMovement = collision_get_slide_vector(&gc->collisionSystem, old_position, movement, playerRadius);
        
        // Apply slide movement if it's significant
        if (Vector3Length(slideMovement) > 0.01f) {
            Vector3 newPos = Vector3Add(old_position, slideMovement);
            
            // Check if slide movement is valid
            if (collision_can_move_to_position(&gc->collisionSystem, old_position, newPos, playerRadius)) {
                gc->player.position = newPos;
            } else {
                // Revert to old position if sliding also fails
                gc->player.position = old_position;
            }
        } else {
            // Revert to old position
            gc->player.position = old_position;
        }
        
        TraceLog(LOG_INFO, "Player collision detected - movement blocked/adjusted");
    }
    
    // Update bounding box after position adjustment
    gc->player.bbox = player_get_bbox(&gc->player);
    
    // Check collision with enemies (keep existing logic)
    bool enemyCollision = false;
    for (int i = 0; i < gc->enemy_count; i++) {
        if (gc->enemies[i].hp > 0) {
            BoundingBox enemy_bbox = enemy_get_bbox(&gc->enemies[i]);
            if (CheckCollisionBoxes(gc->player.bbox, enemy_bbox)) {
                enemyCollision = true;
                gc->enemies[i].hp -= 1.0f;
                break;
            }
        }
    }
    
    if (enemyCollision) {
        gc->player.color = RED;
    } else {
        gc->player.color = WHITE;
    }
}

void player_update(game_context *gc) {
  Vector3 old_position = gc->player.position;
  bool moved = false;
  Vector3 movement = {0.0f, 0.0f, 0.0f};

  // Handle input
  player_handle_input(gc, &movement, &moved);

  // Apply movement
  gc->player.position.x += movement.x;
  gc->player.position.z += movement.z;

  // Update rotation based on movement
  if (moved && (movement.x != 0.0f || movement.z != 0.0f)) {
    gc->player.rotation_y = atan2f(movement.x, movement.z) * RAD2DEG;
  }

  // Handle animations
  player_handle_animation(&gc->player, moved);

  // Handle collisions
  player_handle_collision(gc, old_position);
}

void player_draw(const player_t *player, Shader lightingShader) {
  // Debug: Check if shader is valid
  static int debugCounter = 0;
  if (debugCounter % 300 == 0) { // Every 5 seconds
    TraceLog(LOG_INFO, "Drawing player with shader ID: %d", lightingShader.id);
  }
  debugCounter++;

  // Create transformation matrices - reduced scale for smaller player
  Matrix directionRotation = MatrixRotateY(player->rotation_y * DEG2RAD);
  Matrix scaleMatrix =
      MatrixScale(0.6f, 0.6f, 0.6f); // Reduced from 1.0f to 0.8f
  Matrix translationMatrix = MatrixTranslate(
      player->position.x, player->position.y, player->position.z);

  // Combine transformations (removed uprightRotation)
  Matrix transform = MatrixMultiply(scaleMatrix, directionRotation);
  transform = MatrixMultiply(transform, translationMatrix);

  // Draw main character model
  Model model = player->model;
  for (int i = 0; i < model.meshCount; i++) {
    if (lightingShader.id > 0) {
      // Create a temporary material with the lighting shader
      Material tempMaterial = model.materials[model.meshMaterial[i]];
      tempMaterial.shader = lightingShader;

      // Apply player color to the material
      tempMaterial.maps[MATERIAL_MAP_DIFFUSE].color = player->color;

      DrawMesh(model.meshes[i], tempMaterial, transform);
    } else {
      TraceLog(LOG_WARNING, "Invalid lighting shader, using default");
      // Fallback to default material
      Material tempMaterial = model.materials[model.meshMaterial[i]];
      tempMaterial.maps[MATERIAL_MAP_DIFFUSE].color = player->color;
      DrawMesh(model.meshes[i], tempMaterial, transform);
    }
  }

  // Draw accessories at bone socket positions
  if (player->animsCount > 0 && player->animId < player->animsCount) {
    ModelAnimation currentAnim = player->anims[player->animId];

    for (int i = 0; i < BONE_SOCKETS; i++) {
      if (player->showEquip[i] && player->boneSocketIndex[i] >= 0) {
        // Get bone transform from current animation frame
        Transform boneTransform =
            currentAnim.framePoses[player->animFrameCounter]
                                  [player->boneSocketIndex[i]];

        // Convert bone transform to matrix
        Matrix boneMatrix = MatrixMultiply(
            MatrixMultiply(MatrixScale(boneTransform.scale.x,
                                       boneTransform.scale.y,
                                       boneTransform.scale.z),
                           QuaternionToMatrix(boneTransform.rotation)),
            MatrixTranslate(boneTransform.translation.x,
                            boneTransform.translation.y,
                            boneTransform.translation.z));

        // Combine with character transform
        Matrix accessoryTransform = MatrixMultiply(boneMatrix, transform);

        // Draw accessory model
        Model accessoryModel = player->equipModels[i];
        for (int j = 0; j < accessoryModel.meshCount; j++) {
          if (lightingShader.id > 0) {
            Material tempMaterial =
                accessoryModel.materials[accessoryModel.meshMaterial[j]];
            tempMaterial.shader = lightingShader;
            DrawMesh(accessoryModel.meshes[j], tempMaterial,
                     accessoryTransform);
          } else {
            Material tempMaterial =
                accessoryModel.materials[accessoryModel.meshMaterial[j]];
            DrawMesh(accessoryModel.meshes[j], tempMaterial,
                     accessoryTransform);
          }
        }
      }
    }
  }
}

void player_cleanup(player_t *player) {
  if (player->anims != NULL) {
    UnloadModelAnimations(player->anims, player->animsCount);
    player->anims = NULL;
  }
  UnloadModel(player->model);

  // Cleanup accessory models
  for (int i = 0; i < BONE_SOCKETS; i++) {
    UnloadModel(player->equipModels[i]);
  }
}