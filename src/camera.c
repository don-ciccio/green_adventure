#include "camera.h"
#include <raylib.h>
#include <raymath.h>
#include <math.h>

void camera_init(game_context *gc) {
    // Initialize orthographic camera
    gc->camera.position = (Vector3){15.0f, 20.0f, 15.0f};
    gc->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    gc->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    gc->camera.fovy = 45.0f;
    gc->camera.projection = CAMERA_ORTHOGRAPHIC;
    
    // Initialize camera mode settings
    gc->cameraMode = GAME_CAMERA_MODE_ORTHOGRAPHIC;
    gc->isIndoors = false;
    gc->thirdPersonOffset = (Vector3){0.0f, 3.0f, 5.0f};
    gc->transitionSpeed = 2.0f;
    
    gc->camera_distance = CAMERA_INITIAL_DISTANCE;
    gc->camera_angle = 0.0f;
}

void camera_update(game_context *gc) {
    // Check if player is in an indoor area
    bool wasIndoors = gc->isIndoors;
    gc->isIndoors = camera_is_indoor_position(gc, gc->player.position);
    
    // Switch camera mode if indoor status changed
    if (gc->isIndoors && !wasIndoors) {
        camera_set_mode(gc, GAME_CAMERA_MODE_THIRD_PERSON);
        TraceLog(LOG_INFO, "Switched to third-person camera (indoors)");
    } else if (!gc->isIndoors && wasIndoors) {
        camera_set_mode(gc, GAME_CAMERA_MODE_ORTHOGRAPHIC);
        TraceLog(LOG_INFO, "Switched to orthographic camera (outdoors)");
    }
    
    // Update camera based on current mode
    if (gc->cameraMode == GAME_CAMERA_MODE_ORTHOGRAPHIC) {
        camera_update_orthographic(gc);
    } else {
        camera_update_third_person(gc);
    }
}

void camera_set_mode(game_context *gc, GameCameraMode mode) {
    gc->cameraMode = mode;
    
    if (mode == GAME_CAMERA_MODE_ORTHOGRAPHIC) {
        gc->camera.projection = CAMERA_ORTHOGRAPHIC;
        gc->camera.fovy = gc->camera_distance; // Use distance as orthographic size
    } else {
        gc->camera.projection = CAMERA_PERSPECTIVE;
        gc->camera.fovy = 45.0f;
    }
}

void camera_update_orthographic(game_context *gc) {
    // Handle zoom with mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        gc->camera_distance -= wheel * 2.0f;
        gc->camera_distance = Clamp(gc->camera_distance, CAMERA_MIN_DISTANCE, CAMERA_MAX_DISTANCE);
        gc->camera.fovy = gc->camera_distance;
    }
    
    // Fixed orthographic camera position relative to player
    Vector3 offset = {10.0f, 20.0f, 10.0f};
    gc->camera.position = Vector3Add(gc->player.position, offset);
    gc->camera.target = gc->player.position;
}

void camera_update_third_person(game_context *gc) {
    // Handle zoom with mouse wheel for third-person
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        gc->thirdPersonOffset.z += wheel * 0.5f;
        gc->thirdPersonOffset.z = Clamp(gc->thirdPersonOffset.z, 2.0f, 8.0f);
    }
    
    // Third-person camera behind and above player
    Vector3 targetPos = Vector3Add(gc->player.position, gc->thirdPersonOffset);
    
    // Smooth camera transition
    float deltaTime = GetFrameTime();
    gc->camera.position = Vector3Lerp(gc->camera.position, targetPos, gc->transitionSpeed * deltaTime);
    gc->camera.target = Vector3Lerp(gc->camera.target, gc->player.position, gc->transitionSpeed * deltaTime);
}

bool camera_is_indoor_position(game_context *gc, Vector3 position) {
    // Check if player position is within any indoor custom bounds
    for (int i = 0; i < gc->customBoundCount; i++) {
        CustomBound *bound = &gc->customBounds[i];
        if (!bound->enabled) continue;
        
        // Check if this is an indoor trigger (porch, kitchen, room, etc.)
        if (strcmp(bound->name, "Porch") == 0 || 
            strcmp(bound->name, "Kitchen") == 0 ||
            strcmp(bound->name, "Room1") == 0 ||
            strstr(bound->name, "Room") != NULL) {
            
            // Check if player is within this bound
            Vector3 halfSize = {bound->size.x/2, bound->size.y/2, bound->size.z/2};
            
            if (position.x >= bound->position.x - halfSize.x &&
                position.x <= bound->position.x + halfSize.x &&
                position.y >= bound->position.y - halfSize.y &&
                position.y <= bound->position.y + halfSize.y &&
                position.z >= bound->position.z - halfSize.z &&
                position.z <= bound->position.z + halfSize.z) {
                return true;
            }
        }
    }
    return false;
}

void camera_set_target(game_context *gc, Vector3 target) {
    gc->camera.target = target;
}