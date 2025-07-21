#include "camera.h"

void camera_init(game_context *gc) {
  // Set up orthographic camera positioned above and behind the player
  gc->camera.position = (Vector3){10.0f, 20.0f, 25.0f}; // Start above the house
  gc->camera.target = (Vector3){10.0f, 0.0f, 10.0f};     // Look at house center
  gc->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  gc->camera.fovy = 20.0f; // This becomes the orthographic width/height ratio
  gc->camera.projection = CAMERA_ORTHOGRAPHIC;

  gc->camera_distance = 20.0f; // Fixed distance for orthographic
  gc->camera_angle = 0.0f;     // No rotation needed
}

void camera_update(game_context *gc) {
  // Fixed isometric angle (45 degrees from horizontal, looking down)
  float camera_height = 20.0f;
  float camera_offset = 15.0f; // Distance behind player
  
  // Camera follows player with fixed offset (isometric style)
  gc->camera.position = (Vector3){
    gc->player.position.x - camera_offset * 0.7071f, // 45-degree angle offset
    gc->player.position.y + camera_height,
    gc->player.position.z + camera_offset * 0.7071f
  };
  
  // Always look at the player
  gc->camera.target = gc->player.position;
  
  // Handle zoom with mouse wheel (changes orthographic size)
  float wheel = GetMouseWheelMove();
  gc->camera.fovy -= wheel * 2.0f;
  
  // Clamp orthographic size
  if (gc->camera.fovy < 5.0f) {
    gc->camera.fovy = 5.0f;
  }
  if (gc->camera.fovy > 40.0f) {
    gc->camera.fovy = 40.0f;
  }
}

void camera_set_target(game_context *gc, Vector3 target) {
  gc->camera.target = target;
}