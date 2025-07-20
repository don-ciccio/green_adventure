#ifndef LIGHTING_H
#define LIGHTING_H

#include "game_types.h"

// Initialize lighting system
void lighting_init(game_context *gc);

// Create a light
Light CreateLight(int type, Vector3 position, Vector3 target, Color color,
                  Shader shader, int index);

// Update light values in shader
void UpdateLightValues(Shader shader, Light light, int index);

// Cleanup lighting
void lighting_cleanup(game_context *gc);

#endif