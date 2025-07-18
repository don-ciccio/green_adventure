#include "../lib/raylib.h"
#include "../lib/raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 1600
#define HEIGHT 900
#define ENTITY_LIMIT 256

typedef struct {
  Vector3 position;
  Vector3 size;
  Color color;
  float speed;
  float hp;
  BoundingBox bbox;
} enemy_t;

typedef struct {
  Vector3 position;
  Vector3 size;
  Color color;
  Model model;
  ModelAnimation *anims;
  int animsCount;
  int animFrameCounter;
  int animId;
  float rotation_y;
  float move_speed;
  BoundingBox bbox;
} player_t;

typedef struct {
  Camera camera;
  player_t player;
  enemy_t enemies[ENTITY_LIMIT];
  int num_enemies;
  bool paused;
  bool running;
  float camera_distance;
  float camera_angle;
} game_context;

BoundingBox get_player_bbox(player_t player) {
  return (BoundingBox){.min = (Vector3){player.position.x - player.size.x / 2,
                                        player.position.y - player.size.y / 2,
                                        player.position.z - player.size.z / 2},
                       .max = (Vector3){player.position.x + player.size.x / 2,
                                        player.position.y + player.size.y / 2,
                                        player.position.z + player.size.z / 2}};
}

BoundingBox get_enemy_bbox(enemy_t enemy) {
  return (BoundingBox){.min = (Vector3){enemy.position.x - enemy.size.x / 2,
                                        enemy.position.y - enemy.size.y / 2,
                                        enemy.position.z - enemy.size.z / 2},
                       .max = (Vector3){enemy.position.x + enemy.size.x / 2,
                                        enemy.position.y + enemy.size.y / 2,
                                        enemy.position.z + enemy.size.z / 2}};
}

void load_game(game_context *gc) {
  // Initialize camera
  gc->camera.position = (Vector3){0.0f, 10.0f, 10.0f};
  gc->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  gc->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  gc->camera.fovy = 45.0f;
  gc->camera.projection = CAMERA_PERSPECTIVE;

  // Initialize player
  gc->player.position = (Vector3){0.0f, 1.0f, 2.0f};
  gc->player.size = (Vector3){1.0f, 2.0f, 1.0f};
  gc->player.color = WHITE;
  gc->player.rotation_y = 0.0f;
  gc->player.move_speed =
      0.06f; // Reduced from 0.2f to 0.05f for better animation sync

  // Load player model (M3D format)
  char modelFileName[] = "./assets/cesium_man.m3d";
  gc->player.model = LoadModel(modelFileName);

  // Load animations
  gc->player.animsCount = 0;
  gc->player.animFrameCounter = 0;
  gc->player.animId = 0;
  gc->player.anims = LoadModelAnimations(modelFileName, &gc->player.animsCount);

  // Initialize camera settings
  gc->camera_distance = 8.0f;
  gc->camera_angle = 0.0f;

  // Initialize enemies
  gc->num_enemies = 10;
  for (int i = 0; i < gc->num_enemies; i++) {
    gc->enemies[i].position =
        (Vector3){(float)(i * 2 - 10), 1.0f, (float)(rand() % 20 - 10)};
    gc->enemies[i].size = (Vector3){2.0f, 2.0f, 2.0f};
    gc->enemies[i].color = BLUE;
    gc->enemies[i].speed = 0.1f;
    gc->enemies[i].hp = 100.0f;
    gc->enemies[i].bbox = get_enemy_bbox(gc->enemies[i]);
  }

  gc->paused = false;
  gc->running = true;
}

void update_player(game_context *gc) {
  Vector3 old_position = gc->player.position;
  bool moved = false;
  Vector3 movement = {0.0f, 0.0f, 0.0f};
  float target_rotation = gc->player.rotation_y;

  // Player movement with arrow keys or WASD (with diagonal support)
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
    movement.x += gc->player.move_speed; // Move right (positive X)
    target_rotation = 90.0f;
    moved = true;
  }
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
    movement.x -= gc->player.move_speed; // Move left (negative X)
    target_rotation = -90.0f;
    moved = true;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    movement.z -= gc->player.move_speed; // Move backward (negative Z)
    target_rotation = 180.0f;
    moved = true;
  }
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    movement.z += gc->player.move_speed; // Move forward (positive Z)
    target_rotation = 0.0f;
    moved = true;
  }

  // Apply movement
  gc->player.position.x += movement.x;
  gc->player.position.z += movement.z;

  // Calculate diagonal rotation angles
  if (moved) {
    if (movement.x != 0.0f && movement.z != 0.0f) {
      // Diagonal movement - calculate angle based on movement vector
      gc->player.rotation_y = atan2f(movement.x, movement.z) * RAD2DEG;
    } else {
      // Single direction movement
      gc->player.rotation_y = target_rotation;
    }
  }

  // Animation controls (from M3D example)
  if (gc->player.animsCount > 0) {
    // Play animation when moving or spacebar is held down
    if (moved || IsKeyDown(KEY_SPACE)) {
      gc->player.animFrameCounter++;
      if (gc->player.animFrameCounter >=
          gc->player.anims[gc->player.animId].frameCount) {
        gc->player.animFrameCounter = 0;
      }
      UpdateModelAnimation(gc->player.model,
                           gc->player.anims[gc->player.animId],
                           gc->player.animFrameCounter);
    }

    // Select animation by pressing C
    if (IsKeyPressed(KEY_C)) {
      gc->player.animFrameCounter = 0;
      gc->player.animId++;
      if (gc->player.animId >= gc->player.animsCount) {
        gc->player.animId = 0;
      }
      UpdateModelAnimation(gc->player.model,
                           gc->player.anims[gc->player.animId], 0);
    }
  }

  // Update player bounding box
  gc->player.bbox = get_player_bbox(gc->player);

  bool collision = false;
  for (int i = 0; i < gc->num_enemies; i++) {
    if (gc->enemies[i].hp > 0) {
      BoundingBox enemy_bbox = get_enemy_bbox(gc->enemies[i]);
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

void update_camera(game_context *gc) {
  // Camera follows player with mouse control
  Vector2 mouse_delta = GetMouseDelta();
  gc->camera_angle += mouse_delta.x * 0.01f;

  // Calculate camera position around player
  float cam_x =
      gc->player.position.x + cosf(gc->camera_angle) * gc->camera_distance;
  float cam_z =
      gc->player.position.z + sinf(gc->camera_angle) * gc->camera_distance;

  gc->camera.position = (Vector3){cam_x, gc->player.position.y + 5.0f, cam_z};
  gc->camera.target = gc->player.position;

  // Zoom with mouse wheel
  float wheel = GetMouseWheelMove();
  gc->camera_distance -= wheel;
  if (gc->camera_distance < 3.0f)
    gc->camera_distance = 3.0f;
  if (gc->camera_distance > 15.0f)
    gc->camera_distance = 15.0f;
}

void update_enemies(game_context *gc) {
  for (int i = 0; i < gc->num_enemies; i++) {
    if (gc->enemies[i].hp > 0) {
      // Simple enemy movement
      gc->enemies[i].position.x += gc->enemies[i].speed;
      if (gc->enemies[i].position.x > 15 || gc->enemies[i].position.x < -15) {
        gc->enemies[i].speed *= -1;
      }

      // Update enemy bounding box
      gc->enemies[i].bbox = get_enemy_bbox(gc->enemies[i]);
    }
  }
}

void update_game(game_context *gc) {
  gc->running = !WindowShouldClose();

  if (IsKeyPressed(KEY_ESCAPE)) {
    gc->paused = !gc->paused;
  }

  if (!gc->paused) {
    update_player(gc);
    update_camera(gc);
    update_enemies(gc);
  }
}

void draw_game(game_context *gc) {
  BeginMode3D(gc->camera);

  // Draw grid
  DrawGrid(20, 1.0f);

  // Draw player model with proper M3D rendering
  DrawModelEx(gc->player.model, gc->player.position, (Vector3){0, 1, 0},
              gc->player.rotation_y, (Vector3){1.0f, 1.0f, 1.0f},
              gc->player.color);

  // Draw enemies
  for (int i = 0; i < gc->num_enemies; i++) {
    if (gc->enemies[i].hp > 0) {
      Color enemy_color = gc->enemies[i].color;
      if (gc->enemies[i].hp < 50)
        enemy_color = YELLOW;
      if (gc->enemies[i].hp < 20)
        enemy_color = RED;

      DrawCube(gc->enemies[i].position, gc->enemies[i].size.x,
               gc->enemies[i].size.y, gc->enemies[i].size.z, enemy_color);
      DrawCubeWires(gc->enemies[i].position, gc->enemies[i].size.x,
                    gc->enemies[i].size.y, gc->enemies[i].size.z, DARKGRAY);
    }
  }

  EndMode3D();

  // Draw HUD
  DrawFPS(10, 10);
  DrawText("Move with WASD or Arrow Keys", 10, 40, 20, GRAY);
  DrawText("Mouse to rotate camera, Wheel to zoom", 10, 70, 20, GRAY);
  DrawText("SPACE to play animation, C to change animation", 10, 100, 20, GRAY);
  DrawText("ESC to pause", 10, 130, 20, GRAY);

  if (gc->paused) {
    DrawRectangle(0, 0, WIDTH, HEIGHT, ColorAlpha(BLACK, 0.5f));
    DrawText("PAUSED", WIDTH / 2 - 50, HEIGHT / 2, 40, WHITE);
  }
}

void free_game(game_context *gc) {
  // Unload animations
  if (gc->player.anims != NULL) {
    UnloadModelAnimations(gc->player.anims, gc->player.animsCount);
  }
  UnloadModel(gc->player.model);
}

int main() {
  InitWindow(WIDTH, HEIGHT, "Third Person Game");
  SetTargetFPS(60);

  game_context gc = {0};
  load_game(&gc);

  DisableCursor();

  while (gc.running) {
    update_game(&gc);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_game(&gc);
    EndDrawing();
  }

  free_game(&gc);
  CloseWindow();

  return 0;
}