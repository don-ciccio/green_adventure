#include "camera.h"

void camera_init(game_context *gc) {
  gc->camera.position = (Vector3){0.0f, 10.0f, 10.0f};
  gc->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  gc->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  gc->camera.fovy = 45.0f;
  gc->camera.projection = CAMERA_PERSPECTIVE;

  gc->camera_distance = CAMERA_INITIAL_DISTANCE;
  gc->camera_angle = 0.0f;
}

void camera_update(game_context *gc) {
  Vector2 mouse_delta = GetMouseDelta();

  // Update camera angle with mouse input
  float angle_change = mouse_delta.x * CAMERA_SENSITIVITY;
  float new_angle = gc->camera_angle + angle_change;

  // Clamp camera angle to 180 degrees total rotation
  float max_angle = PI / 2.0f;  // 90 degrees
  float min_angle = -PI / 2.0f; // -90 degrees

  if (new_angle > max_angle) {
    gc->camera_angle = max_angle;
  } else if (new_angle < min_angle) {
    gc->camera_angle = min_angle;
  } else {
    gc->camera_angle = new_angle;
  }

  // Calculate camera position around player
  float cam_x =
      gc->player.position.x + cosf(gc->camera_angle) * gc->camera_distance;
  float cam_z =
      gc->player.position.z + sinf(gc->camera_angle) * gc->camera_distance;

  // Lower camera height from 5.0f to 2.5f
  gc->camera.position = (Vector3){cam_x, gc->player.position.y + 18.3f, cam_z};
  gc->camera.target = gc->player.position;

  // Handle zoom with mouse wheel
  float wheel = GetMouseWheelMove();
  gc->camera_distance -= wheel;

  // Clamp camera distance
  if (gc->camera_distance < CAMERA_MIN_DISTANCE) {
    gc->camera_distance = CAMERA_MIN_DISTANCE;
  }
  if (gc->camera_distance > CAMERA_MAX_DISTANCE) {
    gc->camera_distance = CAMERA_MAX_DISTANCE;
  }
}

void camera_set_target(game_context *gc, Vector3 target) {
  gc->camera.target = target;
}