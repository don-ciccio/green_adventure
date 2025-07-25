#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "scene.h"
#include <raylib.h>

#include "../lib/raylib.h"
#include "../lib/raymath.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Game constants
#define WIDTH 1600
#define HEIGHT 900
#define ENTITY_LIMIT 256
#define PLAYER_MOVE_SPEED 0.12f
#define CAMERA_INITIAL_DISTANCE 20.0f // Fixed distance for orthographic
#define CAMERA_MIN_DISTANCE 15.0f     // Minimum zoom level
#define CAMERA_MAX_DISTANCE 30.0f     // Maximum zoom level
#define CAMERA_SENSITIVITY 0.01f      // Not used in orthographic mode
#define ENEMY_COUNT 10
#define ENEMY_SPEED 0.1f
#define ENEMY_HP 100.0f

// Forward declarations
typedef struct enemy_t enemy_t;
typedef struct player_t player_t;
typedef struct game_context game_context;

// Collision system structures
typedef struct CollisionMesh {
  BoundingBox bbox;
  Vector3 *vertices;
  int vertexCount;
  unsigned short *indices;
  int indexCount;
  Matrix transform;
  char name[64];
} CollisionMesh;

typedef struct CollisionSystem {
  CollisionMesh *meshes;
  int meshCount;
  Model colliderModel;
} CollisionSystem;

// Enemy structure
struct enemy_t {
  Vector3 position;
  Vector3 size;
  Color color;
  float speed;
  float hp;
  BoundingBox bbox;
};

// Add accessory constants
#define BONE_SOCKETS 3
#define BONE_SOCKET_HAT 0
#define BONE_SOCKET_HAND_R 1
#define BONE_SOCKET_HAND_L 2

// Player structure
struct player_t {
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

  // Accessory system
  Model equipModels[BONE_SOCKETS];   // Hat, Sword, Shield
  bool showEquip[BONE_SOCKETS];      // Toggle visibility
  int boneSocketIndex[BONE_SOCKETS]; // Bone indices for sockets
};

// Add these includes at the top
#include <rlgl.h>

// Add lighting constants
#define MAX_LIGHTS 4

// Light types
typedef enum { LIGHT_DIRECTIONAL = 0, LIGHT_POINT } LightType;

// Light structure
typedef struct Light {
  int type;
  bool enabled;
  Vector3 position;
  Vector3 target;
  Color color;

  // Shader locations
  int enabledLoc;
  int typeLoc;
  int positionLoc;
  int targetLoc;
  int colorLoc;
} Light;

// Game context structure
// Add to game_types.h
typedef struct CustomBound {
    Vector3 position;
    Vector3 size;
    Color color;
    char name[32];
    bool enabled;
} CustomBound;

// Add camera mode enum before game_context
typedef enum {
    GAME_CAMERA_MODE_ORTHOGRAPHIC,
    GAME_CAMERA_MODE_THIRD_PERSON
} GameCameraMode;

// Add to game_context structure
struct game_context {
  Camera camera;
  player_t player;
  enemy_t enemies[ENTITY_LIMIT];
  int enemy_count;
  bool paused;
  bool running;
  float camera_distance;
  float camera_angle;

  // Camera mode switching
  GameCameraMode cameraMode;
  bool isIndoors;
  Vector3 thirdPersonOffset;
  float transitionSpeed;

  // Scene system
  SceneId sceneId;
  SceneNodeId doorNodeId;
  SceneModelId doorModelId;

  // Direct door model (for testing)
  Model doorModel;
  Vector3 doorPosition;

  // Lighting system
  Shader lightingShader;
  Light lights[MAX_LIGHTS];
  int lightCount;

  // Collision system
  CollisionSystem collisionSystem;
  
  // Custom bounds for debugging/visualization
  CustomBound customBounds[16];
  int customBoundCount;
};

#endif // GAME_TYPES_H