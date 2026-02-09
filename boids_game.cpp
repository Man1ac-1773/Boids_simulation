/* The gamified version of the boids simulation
 * Each parameter will have a slider/button in the game screen
 * So that one can manipulate it and watch emergent behaviour
 * And can understand the true beauty of flocking simulation
 */

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#define RAYGUI_IMPLEMENTATION

#include "lib/raygui.h"

#define WIDTH 1000
#define HEIGHT 700
#define BOID_COUNT 600

#define TRI_DIM 5.0f // length from center to vertice

#define CAMERA_SPEED 1000.0f;

// namespace to hold all config information
namespace Settings
{
// --- BOID CONTROL ---
float perception_radius = 50.0f;
float max_speed = 2.5f;
float sep_weight = 100.0f;
float ali_weight = 50.0f;
float coh_weight = 40.0f;
float mouse_weight = 5000.0f;
uint WORLD_HEIGHT = 2000;
uint WORLD_WIDTH = 2000;
// --- ---
// --- Settings window params ---
bool menuActive = false;
float menuWidth = 250.0f;
float currentOffset = 0.0f;

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
        if (pos.x > Settings::WORLD_WIDTH)
            pos.x -= Settings::WORLD_WIDTH;
        if (pos.y > Settings::WORLD_HEIGHT)
            pos.y -= Settings::WORLD_HEIGHT;
        if (pos.x < 0)
            pos.x += Settings::WORLD_WIDTH;
        if (pos.y < 0)
            pos.y += Settings::WORLD_HEIGHT;
    }
};

// raygui helpers
void DrawConfig();

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(60);

    std::vector<Boid> boids(BOID_COUNT);

    for (int i = 0; i < BOID_COUNT; i++)
    {
        boids[i].pos = (Vector2) {(float) (rand() % WIDTH), (float) (rand() % HEIGHT)};
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
            Vector2 mouse_sep = boids[i].pos - GetScreenToWorld2D(GetMousePosition(), camera);
            float mouse_dis = Vector2Length(mouse_sep);
            mouse_sep = Vector2Normalize(mouse_sep) * (1.0f / (mouse_dis + 0.001f));
            float deltaTime = GetFrameTime();
            boids[i].vel += ali * Settings::ali_weight * deltaTime + coh * Settings::coh_weight * deltaTime +
                            sep * Settings::sep_weight * deltaTime + mouse_sep * deltaTime * Settings::mouse_weight;

            Vector2ClampValue(boids[i].vel, 0, Settings::max_speed);
            boids[i].pos = boids[i].pos + boids[i].vel;
            boids[i].WrapAroundWorld();
            boids[i].UpdateTriangle();
            DrawTriangle(boids[i].vertices.v1, boids[i].vertices.v3, boids[i].vertices.v2, RAYWHITE);
        }
        DrawRectangleLines(0, 0, Settings::WORLD_HEIGHT, Settings::WORLD_WIDTH, GREEN);
        EndMode2D();
        DrawConfig();
        DrawFPS(WIDTH - 80, 0);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawConfig()
{
    using namespace Settings;

    float target = menuActive ? menuWidth : 0.0f;
    currentOffset += (target - currentOffset) * 0.15f;

    if (currentOffset > 1.0f)
    {
        Rectangle panelArea = {(float) GetScreenWidth() - currentOffset, 0, menuWidth, (float) GetScreenHeight()};

        GuiPanel(panelArea, "BOID CONFIGURATOR");
        float startX = panelArea.x + 60;
        float startY = 50;
        GuiLabel({startX, startY, 120, 20}, "Seperation");
        sep_weight = GuiSliderBar({startX, startY + 20, 120, 20}, "0", "1000", &sep_weight, 0, 1000);
        GuiLabel({startX, startY + 60, 120, 20}, "Alignment");
        ali_weight = GuiSliderBar({startX, startY + 80, 120, 20}, "0", "500", &ali_weight, 0, 500);
        GuiLabel({startX, startY + 120, 120, 20}, "Cohesion");
        coh_weight = GuiSliderBar({startX, startY + 140, 120, 20}, "0", "500", &coh_weight, 0, 500);

        GuiLabel({startX, startY + 180, 120, 20}, "Physics Settings");
        max_speed = GuiSliderBar({startX, startY + 200, 120, 20}, "Max Speed", nullptr, &max_speed, 0.5, 10);

        // showDebug = GuiCheckBox({startX, startY + 180, 20, 20}, "Debug Mode", showDebug);
    }

    float btnX = (float) GetScreenWidth() - currentOffset - 40;
    if (GuiButton({btnX, 10, 30, 30}, menuActive ? ">" : "#141#"))
    {
        menuActive = !menuActive;
    }
}
