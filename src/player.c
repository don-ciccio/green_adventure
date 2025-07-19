#include "player.h"
#include "enemy.h"

void player_init(player_t *player) {
  player->position = (Vector3){0.0f, 1.0f, 2.0f};
  player->size = (Vector3){1.0f, 2.0f, 1.0f};
  player->color = WHITE;
  player->rotation_y = 0.0f;
  player->move_speed = PLAYER_MOVE_SPEED;
  player->animsCount = 0;
  player->animFrameCounter = 0;
  player->animId = 0;
  player->anims = NULL;
}

void player_load_model(player_t *player, const char *model_path) {
  player->model = LoadModel(model_path);
  player->anims = LoadModelAnimations(model_path, &player->animsCount);
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
  // Calculate camera-relative movement directions
  Vector3 forward = {-cosf(gc->camera_angle), 0.0f, -sinf(gc->camera_angle)};
  Vector3 right = {sinf(gc->camera_angle), 0.0f, -cosf(gc->camera_angle)};

  // Analog input system for smooth 360-degree movement
  float input_x = 0.0f;
  float input_z = 0.0f;

  // Keyboard input
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
    input_z += 1.0f;
  }
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
    input_z -= 1.0f;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    input_x += 1.0f;
  }
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    input_x -= 1.0f;
  }

  // Gamepad support
  if (IsGamepadAvailable(0)) {
    float gamepad_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float gamepad_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    input_x += gamepad_x;
    input_z -= gamepad_y;
  }

  // Calculate movement direction
  Vector3 input_direction = {0.0f, 0.0f, 0.0f};

  if (input_x != 0.0f || input_z != 0.0f) {
    input_direction.x = (forward.x * input_z) + (right.x * input_x);
    input_direction.z = (forward.z * input_z) + (right.z * input_x);
    *moved = true;
  }

  // Normalize movement for consistent speed
  if (*moved) {
    float magnitude = sqrtf(input_direction.x * input_direction.x +
                            input_direction.z * input_direction.z);
    if (magnitude > 0.0f) {
      input_direction.x /= magnitude;
      input_direction.z /= magnitude;

      movement->x = input_direction.x * gc->player.move_speed;
      movement->z = input_direction.z * gc->player.move_speed;
    }
  }
}

void player_handle_animation(player_t *player, bool moved) {
  if (player->animsCount > 0) {
    // Play animation when moving or spacebar is held
    if (moved || IsKeyDown(KEY_SPACE)) {
      player->animFrameCounter++;
      if (player->animFrameCounter >=
          player->anims[player->animId].frameCount) {
        player->animFrameCounter = 0;
      }
      UpdateModelAnimation(player->model, player->anims[player->animId],
                           player->animFrameCounter);
    }

    // Change animation with C key
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

  bool collision = false;
  for (int i = 0; i < gc->enemy_count; i++) {
    if (gc->enemies[i].hp > 0) {
      BoundingBox enemy_bbox = enemy_get_bbox(&gc->enemies[i]);
      if (CheckCollisionBoxes(gc->player.bbox, enemy_bbox)) {
        collision = true;
        gc->enemies[i].hp -= 1.0f;
        break;
      }
    }
  }

  if (collision) {
    gc->player.color = RED;
    gc->player.position = Vector3Lerp(gc->player.position, old_position, 0.5f);
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

void player_draw(const player_t *player) {
  DrawModelEx(player->model, player->position, (Vector3){0, 1, 0},
              player->rotation_y, (Vector3){1.0f, 1.0f, 1.0f}, player->color);
}

void player_cleanup(player_t *player) {
  if (player->anims != NULL) {
    UnloadModelAnimations(player->anims, player->animsCount);
    player->anims = NULL;
  }
  UnloadModel(player->model);
}