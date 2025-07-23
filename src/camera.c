#include "camera.h"

void camera_init(game_context *gc) {
  // Set up orthographic camera positioned above and behind the player
  gc->camera.position = (Vector3){10.0f, 15.0f, 20.0f}; // Closer to player
  gc->camera.target = (Vector3){10.0f, 0.0f, 10.0f};    // Look at house center
  gc->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  gc->camera.fovy = 15.0f; // Smaller field of view for closer feel
  gc->camera.projection = CAMERA_ORTHOGRAPHIC;

  gc->camera_distance = 15.0f; // Reduced distance for closer following
  gc->camera_angle = 0.0f;     // No rotation needed
}

void camera_update(game_context *gc) {
  // Reduced camera height and offset for closer following
  float camera_height = 20.0f; // Lower camera height
  float camera_offset = 8.0f;  // Closer distance behind player
  float follow_speed =
      0.1f; // Smooth interpolation factor (0.0 = no follow, 1.0 = instant)

  // Calculate target camera position
  Vector3 target_position = (Vector3){
      gc->player.position.x - camera_offset * 0.7071f, // 45-degree angle offset
      gc->player.position.y + camera_height,
      gc->player.position.z + camera_offset * 0.7071f};

  // Smoothly interpolate camera position towards target
  gc->camera.position.x +=
      (target_position.x - gc->camera.position.x) * follow_speed;
  gc->camera.position.y +=
      (target_position.y - gc->camera.position.y) * follow_speed;
  gc->camera.position.z +=
      (target_position.z - gc->camera.position.z) * follow_speed;

  // Smoothly interpolate camera target to follow player
  gc->camera.target.x +=
      (gc->player.position.x - gc->camera.target.x) * follow_speed;
  gc->camera.target.y +=
      (gc->player.position.y - gc->camera.target.y) * follow_speed;
  gc->camera.target.z +=
      (gc->player.position.z - gc->camera.target.z) * follow_speed;

  // Handle zoom with mouse wheel (changes orthographic size)
  float wheel = GetMouseWheelMove();
  gc->camera.fovy -= wheel * 1.5f; // Slightly reduced zoom sensitivity

  // Clamp orthographic size with wider bounds to prevent too close zoom
  if (gc->camera.fovy < 8.0f) {
    gc->camera.fovy = 8.0f; // Increased minimum to prevent too close zoom
  }
  if (gc->camera.fovy > 25.0f) {
    gc->camera.fovy = 25.0f;
  }
}

void camera_set_target(game_context *gc, Vector3 target) {
  gc->camera.target = target;
}