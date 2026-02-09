/* The gamified version of the boids simulation
 * Each parameter will have a slider/button in the game screen
 * So that one can manipulate it and watch emergent behaviour
 * And can understand the true beauty of flocking simulation
 */

#include <iostream>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#define RAYGUI_IMPLEMENTATION

#include "lib/raygui.h"

#define WIDTH 1000
#define HEIGHT 700
#define WORLD_WIDTH 2000
#define WORLD_HEIGHT 2000
#define BOID_COUNT 600  // total boids
#define MOUSE_CONST 100 // a constant to scale mouse_weight
#define WALL_CONST 100  // a constant to scale wall_weight
#define WALL_TOL 100.0f // distance at which wall starts exerting force
#define TRI_DIM 5.0f    // length from center to vertice of boid triangle
#define CAMERA_SPEED 1000.0f

// namespace to hold all config information
namespace Settings
{
// --- BOID CONTROL ---
float perception_radius = 50.0f;
float max_speed = 2.5f;
float sep_weight = 100.0f;
float ali_weight = 50.0f;
float coh_weight = 40.0f;
float mouse_weight = 50.0f;
float wall_weight = 50.0f;
bool WrapAroundWorld = false;
// --- ---
// --- Settings window params ---
bool menuActive = false;
float menuWidth = 250.0f;
float currentOffset = 0.0f;
// --- ---
}; // namespace Settings

// vertices in clock-wise order
typedef struct triangle_vertices
{
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
} Triangle;

class Boid
{
  public:
    Vector2 pos;
    Vector2 vel;
    Triangle vertices;
    Boid() {};
    void UpdateTriangle()
    {
        Vector2 dir = Vector2Scale(Vector2Normalize(vel), TRI_DIM);
        vertices.v1 = pos + dir;
        dir = Vector2Rotate(dir, 120 * DEG2RAD);
        vertices.v2 = pos + dir;
        dir = Vector2Rotate(dir, 120 * DEG2RAD);
        vertices.v3 = pos + dir;
    }
    void WrapAroundWorld()
    {
        if (pos.x > WORLD_WIDTH)
            pos.x -= WORLD_WIDTH;
        if (pos.y > WORLD_HEIGHT)
            pos.y -= WORLD_HEIGHT;
        if (pos.x < 0)
            pos.x += WORLD_WIDTH;
        if (pos.y < 0)
            pos.y += WORLD_HEIGHT;
    }
    void ClampToWorld()
    {
        if (pos.x > WORLD_WIDTH)
            pos.x = WORLD_WIDTH;
        else if (pos.x < 0)
            pos.x = 0;
        if (pos.y > WORLD_HEIGHT)
            pos.y = WORLD_HEIGHT;
        if (pos.y < 0)
            pos.y = 0;
    }
};

// raygui helpers
void DrawConfig();

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(60);

    std::vector<Boid> boids(BOID_COUNT);

    // spawn boids only within screen limit
    for (int i = 0; i < BOID_COUNT; i++)
    {
        boids[i].pos = (Vector2) {(float) (rand() % WORLD_WIDTH), (float) (rand() % WORLD_HEIGHT)};
        boids[i].vel = (Vector2) {((rand() % 100) / 50.0f - 1), ((rand() % 100) / 50.0f - 1)};
    }
    Camera2D camera = {0};
    camera.target = (Vector2) {(float) WIDTH / 2, (float) HEIGHT / 2};
    camera.offset = (Vector2) {(float) WIDTH / 2, (float) HEIGHT / 2};
    camera.zoom = 0.5f;
    camera.rotation = 0.0f;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        camera.zoom += GetMouseWheelMove() * 0.1f;
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f;
        if (IsKeyDown(KEY_D))
            camera.target.x += GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_A))
            camera.target.x -= GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_W))
            camera.target.y -= GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_S))
            camera.target.y += GetFrameTime() * CAMERA_SPEED;

        for (int i = 0; i < BOID_COUNT; i++)
        {
            Vector2 sep = {0, 0}, ali = {0, 0}, coh = {0, 0};
            int count = 0;

            for (int j = 0; j < BOID_COUNT; j++)
            {
                if (i == j)
                    continue;

                float d = Vector2Distance(boids[i].pos, boids[j].pos);
                if (d < Settings::perception_radius && d > 0)
                {
                    Vector2 diff = boids[i].pos - boids[j].pos;
                    sep += (diff * (1.0f) / (d + 0.0001f));
                    ali += boids[j].vel;
                    coh += boids[j].pos;
                    count++;
                }
            }

            if (count > 0)
            {
                ali = (ali * 1.0f / count);
                coh = (coh * 1.0f / count) - boids[i].pos;
            }

            // --- mouse seperation handling ---
            Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 mouse_sep;
            if (mouse_pos.x > WORLD_WIDTH || mouse_pos.y > WORLD_HEIGHT)
                mouse_sep = {0, 0};
            else
                mouse_sep = boids[i].pos - mouse_pos;
            float mouse_dis = Vector2Length(mouse_sep);
            // mouse can only push if within boid detection range
            if (mouse_dis < Settings::perception_radius && mouse_dis > 0)
                mouse_sep = Vector2Normalize(mouse_sep) * (1.0f / (mouse_dis + 0.001f));
            else
                mouse_sep = {0, 0};
            // --- ---
            // --- wall work ---
            Vector2 wall_sep = {0};
            if (boids[i].pos.x >= WORLD_WIDTH - WALL_TOL)
            {
                wall_sep.x = boids[i].pos.x - WORLD_WIDTH;
            }
            if (boids[i].pos.x <= WALL_TOL)
            {
                wall_sep.x = boids[i].pos.x;
            }
            if (boids[i].pos.y >= WORLD_HEIGHT - WALL_TOL)
            {
                wall_sep.y = boids[i].pos.y - WORLD_HEIGHT;
            }
            if (boids[i].pos.y <= WALL_TOL)
            {
                wall_sep.y = boids[i].pos.y;
            }
            float wall_mag = Vector2Length(wall_sep);
            if (!Settings::WrapAroundWorld)
            {
                wall_sep = Vector2Normalize(wall_sep) * (1.0f / (wall_mag + 0.001f));
            }
            else
                wall_sep = {0};
            float deltaTime = GetFrameTime();
            boids[i].vel += ali * Settings::ali_weight * deltaTime + coh * Settings::coh_weight * deltaTime +
                            sep * Settings::sep_weight * deltaTime +
                            mouse_sep * deltaTime * Settings::mouse_weight * MOUSE_CONST +
                            wall_sep * deltaTime * Settings::wall_weight * WALL_CONST;
            boids[i].vel = Vector2ClampValue(boids[i].vel, 0, Settings::max_speed);
            boids[i].pos = boids[i].pos + boids[i].vel;
            if (Settings::WrapAroundWorld)
                boids[i].WrapAroundWorld();
            else
                boids[i].ClampToWorld();
            boids[i].UpdateTriangle();
            DrawTriangle(boids[i].vertices.v1, boids[i].vertices.v3, boids[i].vertices.v2, RAYWHITE);
        }
        DrawRectangleLines(0, 0, WORLD_HEIGHT, WORLD_WIDTH, GREEN);
        EndMode2D();
        DrawConfig();
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawConfig()
{
    using namespace Settings;

    float target = menuActive ? menuWidth : 0.0f;
    currentOffset += (target - currentOffset) * 0.25f;

    if (currentOffset > 1.0f)
    {
        Rectangle panel = {(float) WIDTH - menuWidth, 0, menuWidth, (float) HEIGHT};

        GuiPanel(panel, "BOID CONFIGURATOR");
        float startX = panel.x + 60;
        float startY = 50;
        GuiLabel({startX, startY, 120, 20}, "Seperation");
        GuiSliderBar({startX, startY + 20, 120, 20}, "0", "1000", &sep_weight, 0, 1000);
        GuiLabel({startX, startY + 40, 120, 20}, "Alignment");
        GuiSliderBar({startX, startY + 60, 120, 20}, "0", "500", &ali_weight, 0, 500);
        GuiLabel({startX, startY + 80, 120, 20}, "Cohesion");
        GuiSliderBar({startX, startY + 100, 120, 20}, "0", "500", &coh_weight, 0, 500);
        GuiLabel({startX, startY + 120, 120, 20}, "Mouse fear");
        GuiSliderBar({startX, startY + 140, 120, 20}, "0", "100", &mouse_weight, 0, 100);
        GuiLabel({startX, startY + 160, 120, 20}, "Max Speed");
        GuiSliderBar({startX, startY + 180, 120, 20}, "0.5", "10", &max_speed, 0.5, 10);
        if (GuiToggle({startX, startY + 220, 120, 20}, "Wrap around world?", &WrapAroundWorld))
            ;
        GuiLabel({startX, startY + 240, 120, 20}, "Wall fear");
        GuiSliderBar({startX, startY + 260, 120, 20}, "0", "100", &wall_weight, 0, 100);
    }

    float btnX = (float) GetScreenWidth() - currentOffset - 40;
    if (GuiButton({btnX, 10, 30, 30}, menuActive ? ">" : "#141#"))
    {
        menuActive = !menuActive;
    }
}
