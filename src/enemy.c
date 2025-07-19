#include "enemy.h"

BoundingBox enemy_get_bbox(const enemy_t *enemy) {
  return (BoundingBox){.min = (Vector3){enemy->position.x - enemy->size.x / 2,
                                        enemy->position.y - enemy->size.y / 2,
                                        enemy->position.z - enemy->size.z / 2},
                       .max = (Vector3){enemy->position.x + enemy->size.x / 2,
                                        enemy->position.y + enemy->size.y / 2,
                                        enemy->position.z + enemy->size.z / 2}};
}

void enemy_init_single(enemy_t *enemy, Vector3 position) {
  enemy->position = position;
  enemy->size = (Vector3){2.0f, 2.0f, 2.0f};
  enemy->color = BLUE;
  enemy->speed = ENEMY_SPEED;
  enemy->hp = ENEMY_HP;
  enemy->bbox = enemy_get_bbox(enemy);
}

void enemies_init(game_context *gc) {
  gc->enemy_count = ENEMY_COUNT;
  for (int i = 0; i < gc->enemy_count; i++) {
    Vector3 position = {(float)(i * 2 - 10), 1.0f, (float)(rand() % 20 - 10)};
    enemy_init_single(&gc->enemies[i], position);
  }
}

void enemies_update(game_context *gc) {
  for (int i = 0; i < gc->enemy_count; i++) {  // Changed from num_enemies to enemy_count
    if (gc->enemies[i].hp > 0) {
      // Simple enemy movement
      gc->enemies[i].position.x += gc->enemies[i].speed;

      // Bounce off boundaries
      if (gc->enemies[i].position.x > 15 || gc->enemies[i].position.x < -15) {
        gc->enemies[i].speed *= -1;
      }

      // Update bounding box
      gc->enemies[i].bbox = enemy_get_bbox(&gc->enemies[i]);
    }
  }
}

void enemies_draw(const game_context *gc) {
  for (int i = 0; i < gc->enemy_count; i++) {
    if (gc->enemies[i].hp > 0) {
      Color enemy_color = gc->enemies[i].color;

      // Change color based on health
      if (gc->enemies[i].hp < 50) {
        enemy_color = YELLOW;
      }
      if (gc->enemies[i].hp < 20) {
        enemy_color = RED;
      }

      DrawCube(gc->enemies[i].position, gc->enemies[i].size.x,
               gc->enemies[i].size.y, gc->enemies[i].size.z, enemy_color);
      DrawCubeWires(gc->enemies[i].position, gc->enemies[i].size.x,
                    gc->enemies[i].size.y, gc->enemies[i].size.z, DARKGRAY);
    }
  }
}