#include "../lib/raylib.h"
#include "../lib/raymath.h"
#include <stdbool.h>

#define WIDTH 1600
#define HEIGHT 900
#define DEFAULT_AK_VIEWMODEL                                                   \
  load_cam((Vector3){-0.9, 1.95, 1.6}, (Vector3){-1, 1.95, -0.2})
#define ZOOMED_AK_VIEWMODEL                                                    \
  (Vector3) { 0, 2.2, 1 }

typedef struct animator {
  float animation_time;
  float duration;
} animator_t;

typedef struct weapon {
  Model model;
  Camera cam;
  Vector3 scale;
  Vector3 rotation_axis;
  float rotation_angle;
  Vector3 fixed_pos;

  bool is_spawning;
  bool is_realoading;
  animator_t animator;
} weapon_t;

Camera load_cam(Vector3 position, Vector3 target) {
  Camera cam = {0};
  cam.position = position;
  cam.target = target;
  cam.up = (Vector3){0.0f, 1.0f, 0.0f};
  cam.fovy = 60.0f;
  cam.projection = CAMERA_PERSPECTIVE;
  return cam;
}

void draw_floor(Camera cam) {
  BeginMode3D(cam);
  DrawGrid(40, 1.0f);
  EndMode3D();
}

void draw_weapon(weapon_t weapon) {
  BeginMode3D(weapon.cam);
  DrawModelEx(weapon.model, weapon.fixed_pos, weapon.rotation_axis,
              -weapon.rotation_axis.z, weapon.scale, WHITE);
  EndMode3D();
}
void scale_weapon(weapon_t *weapon, float scale) {
  weapon->scale = (Vector3){scale, scale, scale};
}

void spawn_weapon(weapon_t *weapon) {
  weapon->animator.animation_time += GetFrameTime();
  float t = weapon->animator.animation_time / weapon->animator.duration;
  t = 1 - powf(1 - t, 3);

  if (t >= 1.0f) {
    t = 1.0f;
    weapon->is_spawning = false;
  }

  weapon->cam.position.y = Lerp(3.5f, 1.95f, t);
  weapon->cam.target.y = Lerp(3.5f, 1.95f, t);
}

void reload_weapon(weapon_t *weapon) {
  weapon->animator.animation_time += GetFrameTime();
  float t = weapon->animator.animation_time / weapon->animator.duration;
  t = (1 - powf(1 - t, 3)) / 2;

  if (t >= 1.0f) {
    t = 1.0f;
    weapon->is_realoading = false;
    weapon->rotation_axis = Vector3Zero();
  }
  float angle = sinf(t * PI) * 40.0f;

  weapon->rotation_axis = (Vector3){0, 0, angle};
}

void free_weapon(weapon_t weapon) { UnloadModel(weapon.model); }

void load_ak(weapon_t *ak47) {
  ak47->model = LoadModel("./assets/ak47.glb");
  scale_weapon(ak47, 0.002);
  ak47->cam = DEFAULT_AK_VIEWMODEL;
  ak47->rotation_axis = (Vector3){0, 0, 1};
  ak47->fixed_pos = (Vector3){0, 0.7, 0};
}

int main() {
  InitWindow(WIDTH, HEIGHT, "FPS weapon view");
  SetTargetFPS(60);
  Camera main_cam =
      load_cam((Vector3){0.0f, 2.0f, 4.0f}, (Vector3){0.0f, 2.0f, 3.0f});

  weapon_t ak47 = {0};
  load_ak(&ak47);
  DisableCursor();
  while (!WindowShouldClose()) {
    HideCursor();
    UpdateCamera(&main_cam, CAMERA_FIRST_PERSON);

    BeginDrawing();
    ClearBackground(WHITE);

    draw_floor(main_cam);
    draw_weapon(ak47);

    if (IsKeyDown(KEY_M) && !ak47.is_spawning) {
      ak47.cam.position.y = 3.5f;
      ak47.animator.animation_time = 0.0f;
      ak47.animator.duration = 0.2f;
      ak47.is_spawning = true;
    } else if (IsKeyDown(KEY_R) && !ak47.is_realoading) {
      ak47.animator.animation_time = 0.0f;
      ak47.animator.duration = 0.25f;
      ak47.is_realoading = true;
    }

    if (ak47.is_spawning) {
      spawn_weapon(&ak47);
    }
    if (ak47.is_realoading) {
      reload_weapon(&ak47);
    }
    EndDrawing();
  }
  free_weapon(ak47);
  CloseWindow();
  return 0;
}