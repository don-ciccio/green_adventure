#include "../lib/raylib.h"
#include <math.h>

#define BACKGROUND_COLOR                                                       \
  (Color) { 186, 149, 127 }
#define CAR_COLOR BLACK
#define CAMERA_FOLLOW_THRESH 400
#define MAX_SKIDMARKS 500
#define SKIDMARK_TIME 3

#define CAR_MAX_SPEED 8
#define CAR_MIN_SPEED -6
#define CAR_SPEEDUP 10
#define CAR_SLOWDOWN 0.97f
#define DRIFT_BIAS 15
#define STEERING_SPEED 2
#define MAX_STEERING 4
#define STEER_BACK_SPEED 0.04f
#define DRIFT_ANGLE_DIFF 30

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float lifespan;
    float age;
    Color color;
} Particle;

typedef struct {
    float x, y;
    float speed;
    int direction;
    float angle;
    float drift_angle;
    float steering;
    int width, length;
    Texture2D texture;
    Rectangle texture_rec;
} Car;

typedef struct {
    int width, height;
    Car car;
    Particle particles[MAX_SKIDMARKS];
    int active_particles;
    Camera2D camera;
    Texture2D soil_texture;
} Game;

void DrawWorld(const Game *game) {
    int tile_size_x = game->soil_texture.width;
    int tile_size_y = game->soil_texture.height;

    float half_screen_width = game->width / (2.0f * game->camera.zoom);
    float half_screen_height = game->height / (2.0f * game->camera.zoom);

    float viewport_left = game->camera.target.x - half_screen_width;
    float viewport_right = game->camera.target.x + half_screen_width;
    float viewport_top = game->camera.target.y - half_screen_height;
    float viewport_bottom = game->camera.target.y + half_screen_height;

    viewport_left -= tile_size_x;
    viewport_right += tile_size_x;
    viewport_top -= tile_size_y;
    viewport_bottom += tile_size_y;

    int start_tile_x = floorf(viewport_left / tile_size_x);
    int end_tile_x = ceilf(viewport_right / tile_size_x);
    int start_tile_y = floorf(viewport_top / tile_size_y);
    int end_tile_y = ceilf(viewport_bottom / tile_size_y);

    for (int y = start_tile_y; y <= end_tile_y; y++) {
        for (int x = start_tile_x; x <= end_tile_x; x++) {
            DrawTexture(game->soil_texture, (float)x * tile_size_x, (float)y * tile_size_y, WHITE);
        }
    }
}

void UpdateCar(float dt, Car *car) {

    if (IsKeyDown(KEY_UP)) {
        car->direction = -1;
        car->speed += CAR_SPEEDUP * dt;
        if (car->speed > CAR_MAX_SPEED) car->speed = CAR_MAX_SPEED;
    } else if (IsKeyDown(KEY_DOWN)) {
        car->direction = 1;
        car->speed -= CAR_SPEEDUP * dt;
        if (car->speed < CAR_MIN_SPEED) car->speed = CAR_MIN_SPEED;
    } else {
        car->speed *= CAR_SLOWDOWN;
    }

    if (IsKeyDown(KEY_LEFT)) {
        car->steering -= STEERING_SPEED * dt * fabsf(car->speed);
        if (car->steering < -MAX_STEERING) car->steering = -MAX_STEERING;
    } else if (IsKeyDown(KEY_RIGHT)) {
        car->steering += STEERING_SPEED * dt * fabsf(car->speed);
        if (car->steering > MAX_STEERING) car->steering = MAX_STEERING;
    }

    car->steering *= (1 - STEER_BACK_SPEED);
    car->angle += car->steering;
    car->drift_angle = (car->angle + car->drift_angle * DRIFT_BIAS) / (1 + DRIFT_BIAS);

    float radians = PI * (car->angle - 90) / 180;
    car->x += car->speed * cosf(radians);
    car->y += car->speed * sinf(radians);

    radians = PI * (car->drift_angle - 90) / 180;
    car->x += car->speed * cosf(radians);
    car->y += car->speed * sinf(radians);
}

void UpdateAndDrawParticles(float dt, Game *game) {
    float drift_diff = game->car.drift_angle - game->car.angle;
    bool drifting = fabsf(drift_diff) > DRIFT_ANGLE_DIFF;

    if (drifting) {
        float radians = PI * (game->car.angle - 240) / 180;
        Vector2 tire_pos = { 
            game->car.x + (game->car.length / 2.6f) * cosf(radians), 
            game->car.y + (game->car.length / 2.6f) * sinf(radians) 
        };

        if (game->active_particles < MAX_SKIDMARKS) {
            game->particles[game->active_particles++] = (Particle){
                .position = tire_pos, .velocity = {0, 0}, .radius = 6,
                .lifespan = SKIDMARK_TIME, .age = 0, .color = BLACK
            };
        }

        radians = PI * (game->car.angle - 300) / 180;
        tire_pos.x = game->car.x + (game->car.length / 2.6f) * cosf(radians);
        tire_pos.y = game->car.y + (game->car.length / 2.6f) * sinf(radians);

        if (game->active_particles < MAX_SKIDMARKS) {
            game->particles[game->active_particles++] = (Particle){
                .position = tire_pos, .velocity = {0, 0}, .radius = 6,
                .lifespan = SKIDMARK_TIME, .age = 0, .color = BLACK
            };
        }
    }

    for (int i = 0; i < game->active_particles; i++) {
        game->particles[i].age += dt;
        if (game->particles[i].age >= game->particles[i].lifespan) {
            game->particles[i] = game->particles[--game->active_particles];
            i--;
            continue;
        }
        float alpha = 1.0f - (game->particles[i].age / game->particles[i].lifespan);
        DrawCircleV(game->particles[i].position, game->particles[i].radius, Fade(game->particles[i].color, alpha));
    }
}

void DrawCar(const Car *car) {
    Rectangle car_rec = { car->x, car->y, (float)car->width, (float)car->length };
    Vector2 car_origin = { (float)car->width / 2, (float)car->length / 2 };
    DrawTexturePro(car->texture, car->texture_rec, car_rec, car_origin, car->angle, WHITE);
}

int main() {
    Game game;
    game.width = 1300;
    game.height = 1000;
    InitWindow(game.width, game.height, "Racer");
    SetTargetFPS(60);

    Image soil_image = LoadImage("assets/craftpix-889156-free-racing-game-kit/PNG/Background_Tiles/Soil_Tile.png");
    ImageRotateCW(&soil_image);
    game.soil_texture = LoadTextureFromImage(soil_image);
    UnloadImage(soil_image);

    Image car_image = LoadImage("assets/craftpix-889156-free-racing-game-kit/PNG/Car_1_Main_Positions/Car_1_01.png");
    game.car.texture = LoadTextureFromImage(car_image);
    game.car.texture_rec = (Rectangle){ 0, 0, game.car.texture.width, game.car.texture.height };
    UnloadImage(car_image);

    game.car.width = 80;
    game.car.length = 150;
    game.car.x = game.width / 2.0f - game.car.width / 2.0f;
    game.car.y = game.height / 2.0f - game.car.length / 2.0f;
    game.car.speed = 0;
    game.car.direction = -1;
    game.car.angle = 0;
    game.car.drift_angle = 0;
    game.car.steering = 0;

    game.active_particles = 0;

    game.camera = (Camera2D){
        .offset = { game.width / 2.0f, game.height / 2.0f },
        .target = { game.car.x, game.car.y },
        .rotation = 0,
        .zoom = 1.0,
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        UpdateCar(dt, &game.car);

        game.camera.target.x = game.car.x;
        game.camera.target.y = game.car.y;

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        BeginMode2D(game.camera);

        DrawWorld(&game);
        UpdateAndDrawParticles(dt, &game);
        DrawCar(&game.car);

        EndMode2D();

        EndDrawing();
    }

    UnloadTexture(game.car.texture);
    UnloadTexture(game.soil_texture);

    CloseWindow();

    return 0;
}