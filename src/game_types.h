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
#define PLAYER_MOVE_SPEED 0.05f
#define CAMERA_INITIAL_DISTANCE 8.0f
#define CAMERA_MIN_DISTANCE 3.0f
#define CAMERA_MAX_DISTANCE 15.0f
#define CAMERA_SENSITIVITY 0.01f
#define ENEMY_COUNT 10
#define ENEMY_SPEED 0.1f
#define ENEMY_HP 100.0f

// Forward declarations
typedef struct enemy_t enemy_t;
typedef struct player_t player_t;
typedef struct game_context game_context;

// Enemy structure
struct enemy_t {
  Vector3 position;
  Vector3 size;
  Color color;
  float speed;
  float hp;
  BoundingBox bbox;
};

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
struct game_context {
  Camera camera;
  player_t player;
  enemy_t enemies[ENTITY_LIMIT];
  int enemy_count;
  bool paused;
  bool running;
  float camera_distance;
  float camera_angle;

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
};

#endif // GAME_TYPES_H