#include "raylib.h"
#include "raymath.h"

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
    0,1,2,3,0,4,5,6,7,4,5,1,2,6,7,3
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
Image g_image;
Texture2D g_render_target;

void SetupCamera(MyCamera* camera);
Matrix GetTransform(const MyCamera* camera);
Matrix MatrixViewport();
Vector3 GetCubeTransformedPoint(Vector3 v, Matrix mat);

int main(void) 
{
    InitWindow(g_screen_width, g_screen_height, "ya3dg");
    SetTargetFPS(60);

    SetupCamera(&g_camera);

    g_image = GenImageColor(g_screen_width, g_screen_height, BLACK);
    g_render_target = LoadTextureFromImage(g_image);
    
    while (!WindowShouldClose())
    {
        const float frame_time = (float)GetTime();
        g_camera.eye.x = 10.0f*cosf(frame_time);
        g_camera.eye.y = 0.0f;
        g_camera.eye.z = 10.0f*sinf(frame_time);
        const Matrix transform = GetTransform(&g_camera);

        ImageClearBackground(&g_image, BLACK);
        
        int size = sizeof(indices) / sizeof(indices[0]);
        for(int i = 0; i < size; ++i)
        {
            const Vector3 p1 = GetCubeTransformedPoint(cube[indices[i]], transform);
            const Vector3 p2 = GetCubeTransformedPoint(cube[indices[(i + 1) % size]], transform);
            
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

    if(IsTextureValid(g_render_target))
    {
        UnloadTexture(g_render_target);
    }

    if(IsImageValid(g_image))
    {
        UnloadImage(g_image);
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

Vector3 GetCubeTransformedPoint(Vector3 v, Matrix mat)
{
    const Vector4 v2 = (Vector4){ v.x, v.y, v.z, 1.0f };
    const Matrix viewport = MatrixViewport(g_screen_width, g_screen_height);
    Vector4 transformed = Vector4Transform(v2, mat);
    transformed = Vector4Scale(transformed, 1.0f / transformed.w);
    const Vector4 p2 = Vector4Transform(transformed, viewport);
    const Vector3 p = { p2.x, p2.y, p2.z };
    return p;
}