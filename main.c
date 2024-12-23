#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define TRAJECTORY_POINT_RADIUS 1
#define TRAJECTORY_LENGTH 100

#define G (3*1e5)
#define DAMPNING (0.95)

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float radius;
    float mass;
    Color color;
    Vector2 trajectory[TRAJECTORY_LENGTH];
    size_t current_trajectory_index;
} Body;

typedef struct {
    Body b1;
    Body b2;
} AppState;

void init_app_state(AppState *app) {
    app->b1 = (Body){
        { 100, 200 }, { 150, 0 }, { 0, 0 },
        25, 100, RED,
        { 0 }, -1,
    };

    app->b2 = (Body){
        { 400, 500 }, { -150, 0 }, { 0, 0 },
        15, 50, BLUE,
        { 0 }, -1,
    };
}

void draw_trajectory(Body *body) {
    for (size_t i = 0; i < TRAJECTORY_LENGTH; ++i) {
        DrawCircleV(body->trajectory[i], TRAJECTORY_POINT_RADIUS, body->color);
    }
}

void draw_app(AppState *app) {
    draw_trajectory(&app->b1);
    draw_trajectory(&app->b2);
    DrawCircleV(app->b1.pos, app->b1.radius, app->b1.color);
    DrawCircleV(app->b2.pos, app->b2.radius, app->b2.color);
}

void update_app_state(AppState *app, float dt) {
    float dist = Vector2Distance(app->b1.pos, app->b2.pos);
    Vector2 r = Vector2Scale(Vector2Subtract(app->b1.pos, app->b2.pos),
                             1.0 / dist);

    app->b1.acc = Vector2Scale(r, (-G * app->b2.mass) / (dist * dist));
    app->b2.acc = Vector2Scale(r, (G * app->b1.mass) / (dist * dist));
    /*printf("dist = %f\n", dist);*/
    /*printf("a1_g = %f\n", Vector2Length(app->b1.acc));*/

    app->b1.vel = Vector2Add(app->b1.vel, Vector2Scale(app->b1.acc, dt));
    app->b2.vel = Vector2Add(app->b2.vel, Vector2Scale(app->b2.acc, dt));

    Vector2 b1_prev_pos = app->b1.pos;
    Vector2 b2_prev_pos = app->b2.pos;
    app->b1.pos = Vector2Add(app->b1.pos, Vector2Scale(app->b1.vel, dt));
    app->b2.pos = Vector2Add(app->b2.pos, Vector2Scale(app->b2.vel, dt));

    if (CheckCollisionCircles(app->b1.pos, app->b1.radius,
                              app->b2.pos, app->b2.radius)) {
        app->b1.pos = b1_prev_pos;
        app->b2.pos = b2_prev_pos;

        app->b1.vel = Vector2Scale(app->b1.vel, -1 * DAMPNING);
        app->b2.vel = Vector2Scale(app->b2.vel, -1 * DAMPNING);
    }

    if (app->b1.current_trajectory_index >= TRAJECTORY_LENGTH) {
        for (size_t i = 0; i < TRAJECTORY_LENGTH-1; ++i) {
            app->b1.trajectory[i] = app->b1.trajectory[i+1];
        }
        app->b1.trajectory[TRAJECTORY_LENGTH-1] = app->b1.pos;
    } else {
        app->b1.trajectory[app->b1.current_trajectory_index++] = app->b1.pos;
    }

    if (app->b2.current_trajectory_index >= TRAJECTORY_LENGTH) {
        for (size_t i = 0; i < TRAJECTORY_LENGTH-1; ++i) {
            app->b2.trajectory[i] = app->b2.trajectory[i+1];
        }
        app->b2.trajectory[TRAJECTORY_LENGTH-1] = app->b2.pos;
    } else {
        app->b2.trajectory[app->b2.current_trajectory_index++] = app->b2.pos;
    }
}

int main() {
    AppState app;
    init_app_state(&app);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2-Body Simulation");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        update_app_state(&app, dt);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            draw_app(&app);
        }
        EndDrawing();
    }

    CloseWindow();
}
