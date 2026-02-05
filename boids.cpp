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

#define SEP_W 100.0f // seperation weight
#define ALI_W 50.0f  // alignment weight
#define COH_W 40.0f  // cohesion weight

#define TRI_DIM 5.0f // length from center to vertice

#define CAMERA_SPEED 1000.0f;

typedef struct triangle_vectors
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
};

Vector2 limit(Vector2 v)
{
    float m = Vector2Length(v);
    if (m > MAX_SPEED)
    {
        v.x = (v.x / m) * MAX_SPEED;
        v.y = (v.y / m) * MAX_SPEED;
    }
    return v;
}

void ClampToWorld(Boid* boid)
{
    if (boid->pos.x > WORLD_WIDTH)
        boid->pos.x -= WORLD_WIDTH;
    if (boid->pos.y > WORLD_HEIGHT)
        boid->pos.y -= WORLD_HEIGHT;
    if (boid->pos.x < 0)
        boid->pos.x += WORLD_WIDTH;
    if (boid->pos.y < 0)
        boid->pos.y += WORLD_HEIGHT;
}
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
                    sep += (diff * (1.0f) / (d + 0.01f));
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
            float deltaTime = GetFrameTime();
            boids[i].vel += ali * ALI_W * deltaTime + coh * COH_W * deltaTime + sep * SEP_W * deltaTime;

            boids[i].vel = limit(boids[i].vel);
            boids[i].pos = boids[i].pos + boids[i].vel;

            ClampToWorld(&boids[i]);

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
