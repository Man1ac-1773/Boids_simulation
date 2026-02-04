#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#define WIDTH 1000
#define HEIGHT 700
#define WORLD_WIDTH 2000
#define WORLD_HEIGHT 2000
#define BOID_COUNT 200

#define PERCEPTION_RADIUS 50
#define MAX_SPEED 2.5f

#define SEP_W 100.0f // seperation weight
#define ALI_W 50.0f  // alignment weight
#define COH_W 40.0f  // cohesion weight

#define TRI_DIM 5.0f // length from center to vertice

#define CAMERA_SPEED 1000.0f;
typedef Vector2 Vec2;
typedef struct
{
    Vec2 pos;
    Vec2 vel;
} Boid;

typedef struct triangle_vectors
{
    Vec2 v1;
    Vec2 v2;
    Vec2 v3; // anti-clockwise order
} Triangle;
// let dimension s = 2.0f;
// we go 2.0f from pos in the direction of vel
// then we rotate that by 120, then 120 again and go 2.0f in that side
// use these three points to make triangle
Vec2 limit(Vec2 v, float max)
{
    float m = Vector2Length(v);
    if (m > max)
    {
        v.x = (v.x / m) * max;
        v.y = (v.y / m) * max;
    }
    return v;
}

Triangle UpdateTriangleBoid(Boid boid)
{
    Vec2 dir = Vector2Scale(Vector2Normalize(boid.vel), TRI_DIM);
    Triangle t;
    t.v1 = Vector2Add(boid.pos, dir);
    dir = Vector2Rotate(dir, 120 * DEG2RAD);
    t.v2 = Vector2Add(boid.pos, dir);
    dir = Vector2Rotate(dir, 120 * DEG2RAD);
    t.v3 = Vector2Add(boid.pos, dir);
    return t;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(60);

    Boid boids[BOID_COUNT];

    for (int i = 0; i < BOID_COUNT; i++)
    {
        boids[i].pos = (Vector2) {rand() % WIDTH, rand() % HEIGHT};
        boids[i].vel = (Vector2) {((rand() % 100) / 50.0f - 1), ((rand() % 100) / 50.0f - 1)};
    }
    Camera2D camera = {0};
    camera.target = (Vec2) {(float) WIDTH / 2, (float) HEIGHT / 2};
    camera.offset = (Vec2) {(float) WIDTH / 2, (float) HEIGHT / 2};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        camera.zoom += GetMouseWheelMove() * 0.1f;
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f;
        if (IsKeyDown(KEY_RIGHT))
            camera.target.x += GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_LEFT))
            camera.target.x -= GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_UP))
            camera.target.y -= GetFrameTime() * CAMERA_SPEED;
        if (IsKeyDown(KEY_DOWN))
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
                    Vector2 diff = Vector2Subtract(boids[i].pos, boids[j].pos);
                    sep = Vector2Add(sep, Vector2Scale(diff, 1.0f / (d + 0.01f)));
                    ali = Vector2Add(ali, boids[j].vel);
                    coh = Vector2Add(coh, boids[j].pos);
                    count++;
                }
            }

            if (count > 0)
            {
                ali = Vector2Scale(ali, 1.0f / count);
                coh = Vector2Subtract(Vector2Scale(coh, 1.0f / count), boids[i].pos);
            }
            float deltaTime = GetFrameTime();
            boids[i].vel = Vector2Add(boids[i].vel, Vector2Add(Vector2Add(Vector2Scale(sep, deltaTime * SEP_W),
                                                                          Vector2Scale(ali, deltaTime * ALI_W)),
                                                               Vector2Scale(coh, deltaTime * COH_W)));

            boids[i].vel = limit(boids[i].vel, MAX_SPEED);
            boids[i].pos = Vector2Add(boids[i].pos, boids[i].vel);

            if (boids[i].pos.x < 0)
                boids[i].pos.x += WIDTH;
            if (boids[i].pos.y < 0)
                boids[i].pos.y += HEIGHT;
            if (boids[i].pos.x > WIDTH)
                boids[i].pos.x -= WIDTH;
            if (boids[i].pos.y > HEIGHT)
                boids[i].pos.y -= HEIGHT;

            Triangle boid_triangle = UpdateTriangleBoid(boids[i]);
            DrawTriangle(boid_triangle.v1, boid_triangle.v3, boid_triangle.v2, RAYWHITE);
        }

        EndMode2D();
        DrawFPS(WIDTH - 80, 0);
        DrawText("Boids", 10, 10, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
