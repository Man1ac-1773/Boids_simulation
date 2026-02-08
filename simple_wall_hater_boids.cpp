/* This version of boids is exactly the same as simple_boids.cpp
 * The difference is that walls are treated as obstacles here and are steered
 * away from
 */
#include <iostream>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>
using namespace std;
#define WIDTH 1000
#define HEIGHT 700
#define WORLD_WIDTH 2000
#define WORLD_HEIGHT 2000
#define BOID_COUNT 600

#define PERCEPTION_RADIUS 50
#define MAX_SPEED 2.5f

#define SEP_W 100.0f    // seperation weight
#define ALI_W 50.0f     // alignment weight
#define COH_W 40.0f     // cohesion weight
#define MOUSE_W 5000.0f // mouse weight
#define WALL_W 10000.0f // wall weight
#define WALL_TOL 100.0f // tolerance of wall distance

#define TRI_DIM 5.0f // length from center to vertice

#define CAMERA_SPEED 1000.0f;

typedef struct triangle_vertices
{
    Vector2 v1;
    Vector2 v2;
    Vector2 v3; // clockwise order
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
    void VelocityLimit()
    {
        float m = Vector2Length(vel);
        if (m > MAX_SPEED)
        {
            vel.x = (vel.x / m) * MAX_SPEED;
            vel.y = (vel.y / m) * MAX_SPEED;
        }
    }
};

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(60);

    vector<Boid> boids(BOID_COUNT);

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
                if (d < PERCEPTION_RADIUS && d > 0)
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
            // -- mouse work --
            Vector2 mouse_sep = boids[i].pos - GetScreenToWorld2D(GetMousePosition(), camera);
            float mouse_dis = Vector2Length(mouse_sep);
            mouse_sep = Vector2Normalize(mouse_sep) * (1.0f / (mouse_dis + 0.001f));
            // -- --
            // -- wall work --
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
            wall_sep = Vector2Normalize(wall_sep) * (1.0f / (wall_mag + 0.001f));
            float deltaTime = GetFrameTime();
            boids[i].vel += ali * ALI_W * deltaTime + coh * COH_W * deltaTime + sep * SEP_W * deltaTime +
                            mouse_sep * deltaTime * MOUSE_W + wall_sep * deltaTime * WALL_W;

            boids[i].VelocityLimit();
            boids[i].pos = boids[i].pos + boids[i].vel;
            boids[i].ClampToWorld();
            boids[i].UpdateTriangle();
            DrawTriangle(boids[i].vertices.v1, boids[i].vertices.v3, boids[i].vertices.v2, RAYWHITE);
        }
        DrawRectangleLines(0, 0, WORLD_HEIGHT, WORLD_WIDTH, GREEN);
        EndMode2D();
        DrawFPS(WIDTH - 80, 0);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
