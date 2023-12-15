#include <stdio.h>
#include <time.h>
#include "pixelengine.h"

typedef struct{
	vec3d p[3];
} triangle;

typedef struct{
	triangle *tris;
    int n_tris;
} mesh;

mesh meshCube;
triangle tris[12];
mat4x4 matProj;
float fTheta;

void Update(float fElapsedTime)
{
    ClearScreen();

    mat4x4 matRotZ, matRotX;
    MatZero(&matRotZ);
    MatZero(&matRotX);

    fTheta += 1.0f * fElapsedTime;

    matRotZ.m[0][0] = cosf(fTheta);
    matRotZ.m[0][1] = sinf(fTheta);
    matRotZ.m[1][0] = -sinf(fTheta);
    matRotZ.m[1][1] = cosf(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(fTheta * 0.5f);
    matRotX.m[1][2] = sinf(fTheta * 0.5f);
    matRotX.m[2][1] = -sinf(fTheta * 0.5f);
    matRotX.m[2][2] = cosf(fTheta * 0.5f);
    matRotX.m[3][3] = 1;

    for (int i = 0; i < meshCube.n_tris; i++)
    {
        triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;
        triangle tri = meshCube.tris[i];

        MultiplyMatrixVector(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatrixVector(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatrixVector(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        MultiplyMatrixVector(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

        MultiplyMatrixVector(&triTranslated.p[0], &triProjected.p[0], &matProj);
        MultiplyMatrixVector(&triTranslated.p[1], &triProjected.p[1], &matProj);
        MultiplyMatrixVector(&triTranslated.p[2], &triProjected.p[2], &matProj);

        triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
        triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
        triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
        triProjected.p[0].x *= 0.5f * (float)myWindow.screenWidth;
        triProjected.p[0].y *= 0.5f * (float)myWindow.screenHeight;
        triProjected.p[1].x *= 0.5f * (float)myWindow.screenWidth;
        triProjected.p[1].y *= 0.5f * (float)myWindow.screenHeight;
        triProjected.p[2].x *= 0.5f * (float)myWindow.screenWidth;
        triProjected.p[2].y *= 0.5f * (float)myWindow.screenHeight;

        COLORREF colors[3];

        colors[0] = RGB(0xff, 0x00, 0x00);
        colors[1] = RGB(0x00, 0xff, 0x00);
        colors[2] = RGB(0x00, 0x00, 0xff);

        DrawTriangle(
            (vec2_t){triProjected.p[0].x, triProjected.p[0].y},
            (vec2_t){triProjected.p[1].x, triProjected.p[1].y},
            (vec2_t){triProjected.p[2].x, triProjected.p[2].y}, colors);

        FillTriangle(
            (vec2_t){triProjected.p[0].x, triProjected.p[0].y},
            (vec2_t){triProjected.p[1].x, triProjected.p[1].y},
            (vec2_t){triProjected.p[2].x, triProjected.p[2].y}, colors);
    }


    ShowScreen();
}

void Init()
{
    // SOUTH
    tris[0] =  (triangle){{{0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f}}};
    tris[1] =  (triangle){{{0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f}}};
    // EAST
    tris[2] =  (triangle){{{1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f}}};
    tris[3] =  (triangle){{{1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f}}};
    // NORTH
    tris[4] =  (triangle){{{1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f}}};
    tris[5] =  (triangle){{{1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f}}};
    // WEST
    tris[6] =  (triangle){{{0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f}}};
    tris[7] =  (triangle){{{0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f}}};
    // TOP
    tris[8] =  (triangle){{{0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f}}};
    tris[9] =  (triangle){{{0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f}}};
    // BOTTOM
    tris[10] = (triangle){{{1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f}}};
    tris[11] = (triangle){{{1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f}}};

    meshCube.tris = tris;
    meshCube.n_tris = 12;

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)myWindow.screenHeight / (float)myWindow.screenWidth;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    MatZero(&matProj);
    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = -fFar * fNear / (fFar - fNear);
    matProj.m[2][3] = 1;
    matProj.m[3][3] = 0.0f;
}

int main(int argc, char const *argv[])
{
    Start(800, 600);

    return 0;
}
