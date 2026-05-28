#include "game.h"
#include "raylib.h"
#include "raymath.h"

#include <assert.h>

inline Vector4 Vector4Transform(Vector4 v, Matrix mat)
{
    return QuaternionTransform(v, mat);
}

Vector3 cube[] = {
    // back face
    {-1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    
    // front face
    {-1.0f, -1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
};

int indices[] = {
    0,1,2,
    0,2,3,

    4,5,6,
    4,6,7,

    0,1,5,
    0,5,4,

    3,2,6,
    3,6,7,

    1,2,6,
    1,6,5,

    0,3,7,
    0,7,4,
};

typedef struct _MyCamera
{
    Vector3 eye;
    Vector3 target;
    Vector3 up;
    double fov_y;
    double aspect;
    double near_plane;
    double far_plane;
    bool is_dirty;
} MyCamera;

MyCamera g_camera;
int g_screen_width = 800;
int g_screen_height = 600;
Image g_image;
Texture2D g_render_target;
Matrix g_transform;
Matrix g_viewport;

void UpdateFrame(float time, float frame_time);
void RenderFrame(float time, float frame_time);
void SetupCamera(MyCamera* camera);
Matrix GetTransform(const MyCamera* camera);
Matrix MatrixViewport();
Vector3 GetCubeTransformedPoint(Vector3 v, Matrix mat);
float ToRadians(float degrees);

void InitializeGame(void)
{
    InitWindow(g_screen_width, g_screen_height, "ya3dg");
    SetTargetFPS(60);

    SetupCamera(&g_camera);

    g_image = GenImageColor(g_screen_width, g_screen_height, BLACK);
    g_render_target = LoadTextureFromImage(g_image);

    g_transform = GetTransform(&g_camera);
    g_viewport = MatrixViewport(g_screen_width, g_screen_height);
}

void RunGame(void)
{
    while (!WindowShouldClose())
    {
        const float time = (float)GetTime();
        const float frame_time = (float)GetFrameTime();
        UpdateFrame(time, frame_time);
        RenderFrame(time, frame_time);
    }
}

void CleanupGame(void)
{
    if(IsTextureValid(g_render_target))
    {
        UnloadTexture(g_render_target);
    }

    if(IsImageValid(g_image))
    {
        UnloadImage(g_image);
    }

    CloseWindow();
}

void UpdateFrame(float time, float frame_time)
{
    const float distance = 5.0f;
    g_camera.eye.x = distance * cosf(time * PI / 4.0f);
    g_camera.eye.y = 2.5f;
    g_camera.eye.z = distance * sinf(time * PI / 4.0f);
    g_camera.is_dirty = true;
}

void RenderFrame(float time, float frame_time)
{
    if (g_camera.is_dirty)
    {
         g_transform = GetTransform(&g_camera);
         g_camera.is_dirty = false;
    }

    ImageClearBackground(&g_image, BLACK);
    
    int size = sizeof(indices) / sizeof(indices[0]);
    assert(size % 3 == 0);

    for(int i = 0; i < size; i += 3)
    {
        const Vector3 p1 = GetCubeTransformedPoint(cube[indices[i]], g_transform);
        const Vector3 p2 = GetCubeTransformedPoint(cube[indices[(i + 1)]], g_transform);
        const Vector3 p3 = GetCubeTransformedPoint(cube[indices[(i + 2)]], g_transform);
        
        Vector2 p4 = { p1.x, p1.y };
        Vector2 p5 = { p2.x, p2.y };
        Vector2 p6 = { p3.x, p3.y };
        ImageDrawLineV(&g_image, p4, p5, GREEN);
        ImageDrawLineV(&g_image, p5, p6, GREEN);
        ImageDrawLineV(&g_image, p6, p4, GREEN);
    }

    UpdateTexture(g_render_target, g_image.data);
    
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureV(g_render_target, (Vector2){ 0, 0 }, WHITE);
    DrawFPS(10, 10);
    EndDrawing();
}

void SetupCamera(MyCamera* camera)
{
    camera->eye = (Vector3){ 0.0f, 2.5f, 5.0f };
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fov_y = 60.0;
    camera->aspect = (double)g_screen_width / (double)g_screen_height;
    camera->near_plane = 0.1;
    camera->far_plane = 100.0;
}

Matrix GetTransform(const MyCamera* camera)
{
    const Matrix look_at = MatrixLookAt(camera->eye, camera->target, camera->up);
    const Matrix projection = MatrixPerspective(ToRadians(camera->fov_y), camera->aspect, camera->near_plane, camera->far_plane);
    return MatrixMultiply(look_at, projection);
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

Vector3 GetCubeTransformedPoint(Vector3 v, Matrix mat)
{
    const Vector4 v2 = (Vector4){ v.x, v.y, v.z, 1.0f };
    Vector4 transformed = Vector4Transform(v2, mat);
    transformed = Vector4Scale(transformed, 1.0f / transformed.w);
    const Vector4 p2 = Vector4Transform(transformed, g_viewport);
    const Vector3 p = { p2.x, p2.y, p2.z };
    return p;
}

float ToRadians(float degrees)
{
    return degrees * DEG2RAD;
}
