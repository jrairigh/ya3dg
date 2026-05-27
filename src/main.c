#include "raylib.h"
#include "raymath.h"

#include <stdio.h>

inline Vector4 Vector4Transform(Vector4 v, Matrix mat)
{
    return QuaternionTransform(v, mat);
}

Vector3 cube[] = {
    // back face
    {-1.0f, -1.0f, -4.0f},
    {-1.0f,  1.0f, -4.0f},
    { 1.0f,  1.0f, -4.0f},
    { 1.0f, -1.0f, -4.0f},
    
    // front face
    {-1.0f, -1.0f,  -1.0f},
    {-1.0f,  1.0f,  -1.0f},
    { 1.0f,  1.0f,  -1.0f},
    { 1.0f, -1.0f,  -1.0f},
};

int indices[] = {
    0,1,2,3,0,4,5,6,7,4
};

typedef struct _MyCamera
{
    Vector3 eye;
    Vector3 target;
    Vector3 up;
    double fovY;
    double aspect;
    double nearPlane;
    double farPlane;
} MyCamera;

MyCamera g_camera;
int g_screen_width = 800;
int g_screen_height = 600;

void SetupCamera(MyCamera* camera);
Matrix GetTransform(const MyCamera* camera);
Matrix MatrixViewport();
Vector3 GetCubeTransformedPoint(Vector4 v, Matrix mat);

int main(void) 
{
    InitWindow(g_screen_width, g_screen_height, "ya3dg");
    SetTargetFPS(60);

    SetupCamera(&g_camera);

    const Matrix transform = GetTransform(&g_camera);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        int size = sizeof(indices) / sizeof(indices[0]);
        for(int i = 0; i < size; ++i)
        {
            const Vector3 p1 = GetCubeTransformedPoint((Vector4){ cube[indices[i]].x, cube[indices[i]].y, cube[indices[i]].z, 1.0f }, transform);
            const Vector3 p2 = GetCubeTransformedPoint((Vector4){ cube[indices[(i + 1) % size]].x, cube[indices[(i + 1) % size]].y, cube[indices[(i + 1) % size]].z, 1.0f }, transform);

            Vector2 start = { p1.x, p1.y };
            Vector2 end = { p2.x, p2.y };
            DrawLineV(start, end, GREEN);
        }

        ClearBackground(BLACK);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void SetupCamera(MyCamera* camera)
{
    camera->eye = (Vector3){ 0.0f, 0.0f, 10.0f };
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fovY = 90.0;
    camera->aspect = (double)g_screen_width / (double)g_screen_height;
    camera->nearPlane = 0.1;
    camera->farPlane = 100.0;
}

Matrix GetTransform(const MyCamera* camera)
{
    const Matrix look_at = MatrixLookAt(camera->eye, camera->target, camera->up);
    const Matrix projection = MatrixPerspective(camera->fovY, camera->aspect, camera->nearPlane, camera->farPlane);
    return MatrixMultiply(projection, look_at);
}

Matrix MatrixViewport(int screen_width, int screen_height)
{
    Matrix result = { 0 };

    result.m0 = (float)screen_width / 2.0f;
    result.m5 = -(float)screen_height / 2.0f;
    result.m10 = 1.0f;
    result.m12 = (float)screen_width / 2.0f;
    result.m13 = (float)screen_height / 2.0f;
    result.m15 = 1.0f;

    return result;
}

Vector3 GetCubeTransformedPoint(Vector4 v, Matrix mat)
{
    const Matrix viewport = MatrixViewport(g_screen_width, g_screen_height);
    Vector4 transformed = Vector4Transform(v, mat);
    transformed = Vector4Scale(transformed, 1.0f / transformed.w);
    //DrawText(TextFormat("Vertex %d: (%.2f, %.2f, %.2f, %.2f)", i, transformed.x, transformed.y, transformed.z, transformed.w), 10, 30 + 20*i, 20, RAYWHITE);
    const Vector4 p2 = Vector4Transform(transformed, viewport);
    const Vector3 p = { p2.x, p2.y, p2.z };
    return p;
}