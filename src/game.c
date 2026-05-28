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
    {-1.0f, -1.0f, -4.0f},
    {-1.0f,  1.0f, -4.0f},
    { 1.0f,  1.0f, -4.0f},
    { 1.0f, -1.0f, -4.0f},
    
    // front face
    {-1.0f, -1.0f,  -2.0f},
    {-1.0f,  1.0f,  -2.0f},
    { 1.0f,  1.0f,  -2.0f},
    { 1.0f, -1.0f,  -2.0f},
};

int indices[] = {
    0,1,
    1,2,
    2,3,
    3,0,

    4,5,
    5,6,
    6,7,
    7,4,

    0,4,
    1,5,
    2,6,
    3,7,
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
    const float distance = 10.0f;
    g_camera.eye.x = distance * cosf(PI / 4.0f);
    g_camera.eye.y = 0.0f;
    g_camera.eye.z = distance * sinf(PI / 4.0f);
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
    assert(size % 2 == 0);

    for(int i = 0; i < size; i += 2)
    {
        const Vector3 p1 = GetCubeTransformedPoint(cube[indices[i]], g_transform);
        const Vector3 p2 = GetCubeTransformedPoint(cube[indices[(i + 1) % size]], g_transform);
        
        Vector2 start = { p1.x, p1.y };
        Vector2 end = { p2.x, p2.y };
        ImageDrawLineV(&g_image, start, end, GREEN);
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
    camera->eye = (Vector3){ 0.0f, 0.0f, 10.0f };
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fov_y = 90.0;
    camera->aspect = (double)g_screen_width / (double)g_screen_height;
    camera->near_plane = 0.1;
    camera->far_plane = 100.0;
}

Matrix GetTransform(const MyCamera* camera)
{
    const Matrix look_at = MatrixLookAt(camera->eye, camera->target, camera->up);
    const Matrix projection = MatrixPerspective(camera->fov_y, camera->aspect, camera->near_plane, camera->far_plane);
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

Vector3 GetCubeTransformedPoint(Vector3 v, Matrix mat)
{
    const Vector4 v2 = (Vector4){ v.x, v.y, v.z, 1.0f };
    Vector4 transformed = Vector4Transform(v2, mat);
    transformed = Vector4Scale(transformed, 1.0f / transformed.w);
    const Vector4 p2 = Vector4Transform(transformed, g_viewport);
    const Vector3 p = { p2.x, p2.y, p2.z };
    return p;
}