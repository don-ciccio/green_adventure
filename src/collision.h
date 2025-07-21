#ifndef COLLISION_H
#define COLLISION_H

#include "game_types.h"
#include <raylib.h>

// Initialize collision system by loading colliders.glb
void collision_init(CollisionSystem *collisionSystem);

// Cleanup collision system
void collision_cleanup(CollisionSystem *collisionSystem);

// Check if a point collides with any collision mesh
bool collision_check_point(CollisionSystem *collisionSystem, Vector3 point);

// Check if a bounding box collides with any collision mesh
bool collision_check_bbox(CollisionSystem *collisionSystem, BoundingBox bbox);

// Check if a sphere collides with any collision mesh
bool collision_check_sphere(CollisionSystem *collisionSystem, Vector3 center, float radius);

// Get the closest collision point for a given position
Vector3 collision_get_closest_point(CollisionSystem *collisionSystem, Vector3 point);

// Check collision between a ray and collision meshes
bool collision_raycast(CollisionSystem *collisionSystem, Ray ray, RayCollision *collision);

// NEW: Advanced mesh-based collision detection using raycasting
bool collision_check_mesh_raycast(CollisionSystem *collisionSystem, Vector3 position, Vector3 direction, float distance, RayCollision *hitInfo);

// NEW: Check if player can move to a position using multiple raycasts
bool collision_can_move_to_position(CollisionSystem *collisionSystem, Vector3 currentPos, Vector3 targetPos, float playerRadius);

// NEW: Get collision normal for sliding movement
Vector3 collision_get_slide_vector(CollisionSystem *collisionSystem, Vector3 position, Vector3 movement, float playerRadius);

// Debug: Draw collision bounding boxes
void collision_debug_draw(CollisionSystem *collisionSystem);

// Toggle collision debug visualization
void collision_toggle_debug();

// Check if collision debug is enabled
bool collision_is_debug_enabled();

#endif // COLLISION_H