#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#define WIDTH 1000
#define HEIGHT 700
#define BOID_COUNT 200

#define PERCEPTION_RADIUS 50
#define MAX_SPEED 2.5f

#define SEP_W 1.6f // seperation weight
#define ALI_W 1.0f // alignment weight
#define COH_W 0.8f // cohesion weight

typedef struct
{
    Vector2 pos;
    Vector2 vel;
} Boid;

Vector2 limit(Vector2 v, float max)
{
    float m = Vector2Length(v);
    if (m > max)
    {
        v.x = (v.x / m) * max;
        v.y = (v.y / m) * max;
    }
    return v;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids (raylib)");
    SetTargetFPS(60);

    Boid boids[BOID_COUNT];

    for (int i = 0; i < BOID_COUNT; i++)
    {
        boids[i].pos = (Vector2) {rand() % WIDTH, rand() % HEIGHT};
        boids[i].vel = (Vector2) {((rand() % 100) / 50.0f - 1), ((rand() % 100) / 50.0f - 1)};
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
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

            boids[i].vel =
                Vector2Add(boids[i].vel, Vector2Add(Vector2Add(Vector2Scale(sep, SEP_W), Vector2Scale(ali, ALI_W)),
                                                    Vector2Scale(coh, COH_W)));

            boids[i].vel = limit(boids[i].vel, MAX_SPEED);
            boids[i].pos = Vector2Add(boids[i].pos, boids[i].vel);

            // wrap around
            if (boids[i].pos.x < 0)
                boids[i].pos.x += WIDTH;
            if (boids[i].pos.y < 0)
                boids[i].pos.y += HEIGHT;
            if (boids[i].pos.x > WIDTH)
                boids[i].pos.x -= WIDTH;
            if (boids[i].pos.y > HEIGHT)
                boids[i].pos.y -= HEIGHT;

            DrawCircleV(boids[i].pos, 2, RAYWHITE);
        }

        DrawFPS(WIDTH - 80, 0);
        DrawText("Boids", 10, 10, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
