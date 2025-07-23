#include "lighting.h"
#include <stdio.h>

void lighting_init(game_context *gc) {
  // Load lighting shader
  gc->lightingShader =
      LoadShader("assets/shaders/lighting.vs", "assets/shaders/lighting.fs");

  // Debug: Check if shader loaded successfully
  if (gc->lightingShader.id == 0) {
    TraceLog(LOG_ERROR, "Failed to load lighting shader!");
    return;
  } else {
    TraceLog(LOG_INFO, "Lighting shader loaded successfully (ID: %d)",
             gc->lightingShader.id);
  }

  // Get shader locations
  gc->lightingShader.locs[SHADER_LOC_VECTOR_VIEW] =
      GetShaderLocation(gc->lightingShader, "viewPos");
  TraceLog(LOG_INFO, "viewPos location: %d",
           gc->lightingShader.locs[SHADER_LOC_VECTOR_VIEW]);

  // Set softer ambient light for better texture visibility
  int ambientLoc = GetShaderLocation(gc->lightingShader, "ambient");
  TraceLog(LOG_INFO, "ambient location: %d", ambientLoc);
  SetShaderValue(gc->lightingShader, ambientLoc,
                 (float[4]){0.2f, 0.2f, 0.25f, 1.0f}, SHADER_UNIFORM_VEC4);

  // Create softer, more natural lighting with reduced intensity
  gc->lights[0] = CreateLight(LIGHT_POINT, (Vector3){-15, 12, -15}, Vector3Zero(),
                              (Color){180, 185, 190, 255}, gc->lightingShader, 0);
  gc->lights[1] = CreateLight(LIGHT_POINT, (Vector3){15, 12, 15}, Vector3Zero(),
                              (Color){160, 170, 180, 255}, gc->lightingShader, 1);
  gc->lights[2] =
      CreateLight(LIGHT_DIRECTIONAL, (Vector3){0, 20, 0}, (Vector3){0, 0, 0},
                  (Color){200, 210, 220, 255}, gc->lightingShader, 2);
  gc->lightCount = 3;

  TraceLog(LOG_INFO, "Lighting system initialized with %d lights",
           gc->lightCount);
}

Light CreateLight(int type, Vector3 position, Vector3 target, Color color,
                  Shader shader, int index) {
  Light light = {0};

  light.enabled = true;
  light.type = type;
  light.position = position;
  light.target = target;
  light.color = color;

  // Get shader uniform locations with proper indexing
  char enabledName[32], typeName[32], posName[32], targetName[32],
      colorName[32];
  sprintf(enabledName, "lights[%d].enabled", index);
  sprintf(typeName, "lights[%d].type", index);
  sprintf(posName, "lights[%d].position", index);
  sprintf(targetName, "lights[%d].target", index);
  sprintf(colorName, "lights[%d].color", index);

  light.enabledLoc = GetShaderLocation(shader, enabledName);
  light.typeLoc = GetShaderLocation(shader, typeName);
  light.positionLoc = GetShaderLocation(shader, posName);
  light.targetLoc = GetShaderLocation(shader, targetName);
  light.colorLoc = GetShaderLocation(shader, colorName);

  // Debug output
  TraceLog(LOG_INFO,
           "Light %d created - Type: %s, Position: (%.1f, %.1f, %.1f)", index,
           (type == LIGHT_POINT) ? "POINT" : "DIRECTIONAL", position.x,
           position.y, position.z);
  TraceLog(LOG_INFO,
           "Light %d shader locations - enabled: %d, type: %d, pos: %d, "
           "target: %d, color: %d",
           index, light.enabledLoc, light.typeLoc, light.positionLoc,
           light.targetLoc, light.colorLoc);

  return light;
}

void UpdateLightValues(Shader shader, Light light, int index) {
  // Update light values in shader
  SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
  SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

  float position[3] = {light.position.x, light.position.y, light.position.z};
  SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

  float target[3] = {light.target.x, light.target.y, light.target.z};
  SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

  float color[4] = {
      (float)light.color.r / 255.0f, (float)light.color.g / 255.0f,
      (float)light.color.b / 255.0f, (float)light.color.a / 255.0f};
  SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);

  // Debug output (only occasionally to avoid spam)
  static int debugCounter = 0;
  if (debugCounter % 300 == 0) { // Every 5 seconds at 60 FPS
    TraceLog(LOG_INFO,
             "Light %d updated - Enabled: %s, Color: (%.2f, %.2f, %.2f)", index,
             light.enabled ? "YES" : "NO", color[0], color[1], color[2]);
  }
  debugCounter++;
}

void lighting_cleanup(game_context *gc) {
  TraceLog(LOG_INFO, "Cleaning up lighting system");
  UnloadShader(gc->lightingShader);
}