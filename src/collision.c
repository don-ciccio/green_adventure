#include "collision.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global debug flag
static bool collision_debug_enabled = false;

void collision_init(CollisionSystem *collisionSystem) {
  // Load the colliders model
  collisionSystem->colliderModel = LoadModel("./assets/colliders.glb");

  if (collisionSystem->colliderModel.meshCount == 0) {
    TraceLog(LOG_ERROR, "Failed to load colliders.glb!");
    collisionSystem->meshCount = 0;
    return;
  }

  TraceLog(LOG_INFO, "Loaded colliders.glb with %d meshes",
           collisionSystem->colliderModel.meshCount);

  // Allocate memory for collision meshes
  collisionSystem->meshCount = collisionSystem->colliderModel.meshCount;
  collisionSystem->meshes = (CollisionMesh *)MemAlloc(
      sizeof(CollisionMesh) * collisionSystem->meshCount);

  // Process each mesh in the collider model
  for (int i = 0; i < collisionSystem->meshCount; i++) {
    Mesh mesh = collisionSystem->colliderModel.meshes[i];
    CollisionMesh *collMesh = &collisionSystem->meshes[i];

    // Calculate bounding box
    collMesh->bbox = GetMeshBoundingBox(mesh);

    // Copy vertex data
    collMesh->vertexCount = mesh.vertexCount;
    collMesh->vertices =
        (Vector3 *)MemAlloc(sizeof(Vector3) * mesh.vertexCount);

    for (int v = 0; v < mesh.vertexCount; v++) {
      collMesh->vertices[v] =
          (Vector3){mesh.vertices[v * 3], mesh.vertices[v * 3 + 1],
                    mesh.vertices[v * 3 + 2]};
    }

    // Copy index data if available
    if (mesh.indices) {
      collMesh->indexCount = mesh.triangleCount * 3;
      collMesh->indices = (unsigned short *)MemAlloc(sizeof(unsigned short) *
                                                     collMesh->indexCount);
      memcpy(collMesh->indices, mesh.indices,
             sizeof(unsigned short) * collMesh->indexCount);
    } else {
      collMesh->indexCount = 0;
      collMesh->indices = NULL;
    }

    // Set identity transform (can be modified later for dynamic objects)
    collMesh->transform = MatrixIdentity();

    // Set name
    snprintf(collMesh->name, sizeof(collMesh->name), "Collider_%d", i);

    TraceLog(LOG_INFO,
             "Collision mesh %d: %d vertices, %d indices, bbox: "
             "(%.2f,%.2f,%.2f) to (%.2f,%.2f,%.2f)",
             i, collMesh->vertexCount, collMesh->indexCount,
             collMesh->bbox.min.x, collMesh->bbox.min.y, collMesh->bbox.min.z,
             collMesh->bbox.max.x, collMesh->bbox.max.y, collMesh->bbox.max.z);
  }
}

void collision_cleanup(CollisionSystem *collisionSystem) {
  if (collisionSystem->meshes) {
    for (int i = 0; i < collisionSystem->meshCount; i++) {
      CollisionMesh *mesh = &collisionSystem->meshes[i];
      if (mesh->vertices) {
        MemFree(mesh->vertices);
      }
      if (mesh->indices) {
        MemFree(mesh->indices);
      }
    }
    MemFree(collisionSystem->meshes);
  }

  UnloadModel(collisionSystem->colliderModel);
  collisionSystem->meshCount = 0;
}

bool collision_check_point(CollisionSystem *collisionSystem, Vector3 point) {
  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    // Check if point is inside bounding box
    if (point.x >= mesh->bbox.min.x && point.x <= mesh->bbox.max.x &&
        point.y >= mesh->bbox.min.y && point.y <= mesh->bbox.max.y &&
        point.z >= mesh->bbox.min.z && point.z <= mesh->bbox.max.z) {
      return true;
    }
  }
  return false;
}

bool collision_check_bbox(CollisionSystem *collisionSystem, BoundingBox bbox) {
  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    if (CheckCollisionBoxes(bbox, mesh->bbox)) {
      return true;
    }
  }
  return false;
}

bool collision_check_sphere(CollisionSystem *collisionSystem, Vector3 center,
                            float radius) {
  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    if (CheckCollisionBoxSphere(mesh->bbox, center, radius)) {
      return true;
    }
  }
  return false;
}

Vector3 collision_get_closest_point(CollisionSystem *collisionSystem,
                                    Vector3 point) {
  Vector3 closest = point;
  float minDistance = INFINITY;

  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    // Get closest point on bounding box
    Vector3 boxClosest = {Clamp(point.x, mesh->bbox.min.x, mesh->bbox.max.x),
                          Clamp(point.y, mesh->bbox.min.y, mesh->bbox.max.y),
                          Clamp(point.z, mesh->bbox.min.z, mesh->bbox.max.z)};

    float distance = Vector3Distance(point, boxClosest);
    if (distance < minDistance) {
      minDistance = distance;
      closest = boxClosest;
    }
  }

  return closest;
}

bool collision_raycast(CollisionSystem *collisionSystem, Ray ray,
                       RayCollision *collision) {
  RayCollision closest = {0};
  closest.distance = INFINITY;
  bool hit = false;

  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    RayCollision boxHit = GetRayCollisionBox(ray, mesh->bbox);
    if (boxHit.hit && boxHit.distance < closest.distance) {
      closest = boxHit;
      hit = true;
    }
  }

  if (collision) {
    *collision = closest;
  }

  return hit;
}

void collision_debug_draw(CollisionSystem *collisionSystem) {
  // Only draw if debug is enabled
  if (!collision_debug_enabled) {
    return;
  }

  // Create transform matrix to match house position (10, 0, 10)
  Matrix houseTransform = MatrixTranslate(10.0f, 0.0f, 10.0f);

  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *mesh = &collisionSystem->meshes[i];

    // Draw actual mesh with house transform
    Mesh rayMesh = collisionSystem->colliderModel.meshes[i];
    DrawMesh(rayMesh, LoadMaterialDefault(), houseTransform);

    // Draw wireframe with transformed vertices
    if (mesh->indices && mesh->indexCount > 0) {
      for (int j = 0; j < mesh->indexCount; j += 3) {
        // Get triangle vertices and transform them
        Vector3 v1 =
            Vector3Transform(mesh->vertices[mesh->indices[j]], houseTransform);
        Vector3 v2 = Vector3Transform(mesh->vertices[mesh->indices[j + 1]],
                                      houseTransform);
        Vector3 v3 = Vector3Transform(mesh->vertices[mesh->indices[j + 2]],
                                      houseTransform);

        // Draw triangle edges
        DrawLine3D(v1, v2, RED);
        DrawLine3D(v2, v3, RED);
        DrawLine3D(v3, v1, RED);
      }
    }
  }
}

void collision_toggle_debug() {
  collision_debug_enabled = !collision_debug_enabled;
  TraceLog(LOG_INFO, "Collision debug visualization: %s",
           collision_debug_enabled ? "ON" : "OFF");
}

bool collision_is_debug_enabled() { return collision_debug_enabled; }

// NEW: Advanced mesh-based collision detection using raycasting
bool collision_check_mesh_raycast(CollisionSystem *collisionSystem,
                                  Vector3 position, Vector3 direction,
                                  float distance, RayCollision *hitInfo) {
  Ray ray = {position, direction};
  RayCollision closest = {0};
  closest.distance = INFINITY;
  bool hit = false;

  // Create transform matrix to match house position (10, 0, 10)
  Matrix houseTransform = MatrixTranslate(10.0f, 0.0f, 10.0f);

  for (int i = 0; i < collisionSystem->meshCount; i++) {
    CollisionMesh *collMesh = &collisionSystem->meshes[i];
    Mesh mesh = collisionSystem->colliderModel.meshes[i];

    // Use GetRayCollisionMesh for precise triangle intersection
    RayCollision meshHit = GetRayCollisionMesh(ray, mesh, houseTransform);

    if (meshHit.hit && meshHit.distance <= distance &&
        meshHit.distance < closest.distance) {
      closest = meshHit;
      hit = true;
    }
  }

  if (hitInfo) {
    *hitInfo = closest;
  }

  return hit;
}

// NEW: Check if player can move to a position using multiple raycasts
bool collision_can_move_to_position(CollisionSystem *collisionSystem,
                                    Vector3 currentPos, Vector3 targetPos,
                                    float playerRadius) {
  Vector3 movement = Vector3Subtract(targetPos, currentPos);
  float moveDistance = Vector3Length(movement);

  if (moveDistance < 0.001f)
    return true; // No movement

  Vector3 moveDirection = Vector3Normalize(movement);

  // Cast multiple rays around the player's cylinder
  const int rayCount = 8;
  const float angleStep = 2.0f * PI / rayCount;

  for (int i = 0; i < rayCount; i++) {
    float angle = i * angleStep;
    Vector3 offset = {cosf(angle) * playerRadius, 0.0f,
                      sinf(angle) * playerRadius};

    Vector3 rayStart = Vector3Add(currentPos, offset);
    RayCollision hitInfo;

    // Cast ray in movement direction
    if (collision_check_mesh_raycast(collisionSystem, rayStart, moveDirection,
                                     moveDistance + 0.1f, &hitInfo)) {
      return false; // Collision detected
    }
  }

  // Also check center ray
  RayCollision centerHit;
  if (collision_check_mesh_raycast(collisionSystem, currentPos, moveDirection,
                                   moveDistance + 0.1f, &centerHit)) {
    return false;
  }

  return true; // No collision, movement is safe
}

// NEW: Get collision normal for sliding movement
Vector3 collision_get_slide_vector(CollisionSystem *collisionSystem,
                                   Vector3 position, Vector3 movement,
                                   float playerRadius) {
  Vector3 moveDirection = Vector3Normalize(movement);
  float moveDistance = Vector3Length(movement);

  RayCollision hitInfo;
  if (collision_check_mesh_raycast(collisionSystem, position, moveDirection,
                                   moveDistance + playerRadius, &hitInfo)) {
    // Calculate slide vector along the collision surface
    Vector3 normal = hitInfo.normal;
    Vector3 slideDirection = Vector3Subtract(
        movement, Vector3Scale(normal, Vector3DotProduct(movement, normal)));
    return slideDirection;
  }

  return movement; // No collision, return original movement
}

// Add these functions to collision.c
void collision_add_custom_bound(game_context *gc, Vector3 houseRelativePos, Vector3 size, Color color, const char* name) {
    if (gc->customBoundCount >= 16) return; // Max bounds reached
    
    CustomBound *bound = &gc->customBounds[gc->customBoundCount];
    
    // Convert house-relative position to world position
    bound->position = (Vector3){
        10.0f + houseRelativePos.x,
        0.0f + houseRelativePos.y,
        10.0f + houseRelativePos.z
    };
    
    bound->size = size;
    bound->color = color;
    bound->enabled = true;
    strncpy(bound->name, name, sizeof(bound->name) - 1);
    bound->name[sizeof(bound->name) - 1] = '\0';
    
    gc->customBoundCount++;
    
    TraceLog(LOG_INFO, "Added custom bound '%s' at house position (%.2f, %.2f, %.2f)", 
             name, houseRelativePos.x, houseRelativePos.y, houseRelativePos.z);
}

void collision_draw_bounds_at_position(Vector3 position, Vector3 size, Color color) {
    // Draw filled cube with transparency
    Color fillColor = color;
    fillColor.a = 80; // Semi-transparent
    DrawCube(position, size.x, size.y, size.z, fillColor);
    
    // Draw bright wireframe outline
    DrawCubeWires(position, size.x, size.y, size.z, color);
    
    // Draw corner markers for better visibility
    float markerSize = 0.1f;
    Vector3 halfSize = {size.x/2, size.y/2, size.z/2};
    
    // Draw 8 corner markers
    Vector3 corners[8] = {
        {position.x - halfSize.x, position.y - halfSize.y, position.z - halfSize.z},
        {position.x + halfSize.x, position.y - halfSize.y, position.z - halfSize.z},
        {position.x - halfSize.x, position.y + halfSize.y, position.z - halfSize.z},
        {position.x + halfSize.x, position.y + halfSize.y, position.z - halfSize.z},
        {position.x - halfSize.x, position.y - halfSize.y, position.z + halfSize.z},
        {position.x + halfSize.x, position.y - halfSize.y, position.z + halfSize.z},
        {position.x - halfSize.x, position.y + halfSize.y, position.z + halfSize.z},
        {position.x + halfSize.x, position.y + halfSize.y, position.z + halfSize.z}
    };
    
    for (int i = 0; i < 8; i++) {
        DrawCube(corners[i], markerSize, markerSize, markerSize, WHITE);
    }
}

void collision_draw_custom_bounds(game_context *gc) {
    for (int i = 0; i < gc->customBoundCount; i++) {
        CustomBound *bound = &gc->customBounds[i];
        if (bound->enabled) {
            // Enhanced drawing with highlighting
            collision_draw_bounds_at_position(bound->position, bound->size, bound->color);
            
            // Draw name label with background
            Vector2 screenPos = GetWorldToScreen(bound->position, gc->camera);
            if (screenPos.x >= 0 && screenPos.x < GetScreenWidth() && 
                screenPos.y >= 0 && screenPos.y < GetScreenHeight()) {
                
                // Draw text background
                int textWidth = MeasureText(bound->name, 16);
                DrawRectangle((int)screenPos.x - 5, (int)screenPos.y - 25, textWidth + 10, 20, BLACK);
                
                // Draw text
                DrawText(bound->name, (int)screenPos.x, (int)screenPos.y - 20, 16, bound->color);
                
                // Draw position info
                char posText[64];
                snprintf(posText, sizeof(posText), "(%.1f, %.1f, %.1f)", 
                        bound->position.x - 10.0f, bound->position.y, bound->position.z - 10.0f);
                DrawText(posText, (int)screenPos.x, (int)screenPos.y - 5, 12, LIGHTGRAY);
            }
        }
    }
}