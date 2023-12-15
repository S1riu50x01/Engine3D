#ifndef _PIXELENGINE_H_
#define _PIXELENGINE_H_

#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"

// Definir as dimensões da janela e do buffer

struct Window {
    HWND hwnd;
    unsigned int screenWidth;
    unsigned int screenHeight;
    BYTE *buffer;
} myWindow;

void Update(float);
void Init(void);

// Função de desenho para preencher o buffer
void PutPixel(vec2_t v, COLORREF color) {   
    if ( (int)v.x > 0 && (int)v.x < myWindow.screenWidth && (int)v.y > 0 && (int)v.y < myWindow.screenHeight)
    {
        // Cada pixel é representado por 3 bytes (RGB)
        myWindow.buffer[((int)v.y * myWindow.screenWidth + (int)v.x) * 3 + 2] = GetRValue(color); // Componente vermelha
        myWindow.buffer[((int)v.y * myWindow.screenWidth + (int)v.x) * 3 + 1] = GetGValue(color);   // Componente verde
        myWindow.buffer[((int)v.y * myWindow.screenWidth + (int)v.x) * 3 + 0] = GetBValue(color);   // Componente azul
    }
        
}

COLORREF InterpolateColor(COLORREF col1, COLORREF col2, float t) {
    BYTE r1 = GetRValue(col1);
    BYTE g1 = GetGValue(col1);
    BYTE b1 = GetBValue(col1);

    BYTE r2 = GetRValue(col2);
    BYTE g2 = GetGValue(col2);
    BYTE b2 = GetBValue(col2);

    BYTE r = (BYTE)((1 - t) * r1 + t * r2);
    BYTE g = (BYTE)((1 - t) * g1 + t * g2);
    BYTE b = (BYTE)((1 - t) * b1 + t * b2);

    return RGB(r, g, b);
}

void DrawLine(vec2_t v1, vec2_t v2, COLORREF c1, COLORREF c2) {
    // Calcular as diferenças entre os pontos inicial e final
    int x1 = v1.x; int x2 = v2.x;
    int y1 = v1.y; int y2 = v2.y;
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Determinar os passos para incrementar/decrementar x e y
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;

    // Desenhar pixels ao longo da linha
    float x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        vec2_t p = {x, y};
        p.x += 1;
        p.y += 1;
        COLORREF currentColor = InterpolateColor(c1, c2, i / (float)steps);

        PutPixel(p, currentColor);
        x += xIncrement;
        y += yIncrement;
    }
}

void DrawTriangle(vec2_t v0, vec2_t v1, vec2_t v2, COLORREF *colors) {
    DrawLine(v0, v1, colors[0], colors[1]);
    DrawLine(v1, v2, colors[1], colors[2]);
    DrawLine(v2, v0, colors[2], colors[0]);
}

int IsTopLeft(vec2_t start, vec2_t end) {
    vec2_t edge = {end.x - start.x, end.y - start.y};
    int is_top_edge = edge.y == 0 && edge.x > 0;
    int is_left_edge = edge.y < 0;
    return is_top_edge || is_left_edge;
}

float EdgeCross(vec2_t a, vec2_t b, vec2_t p) {
    vec2_t ab = { b.x - a.x, b.y - a.y};
    vec2_t ap = { p.x - a.x, p.y - a.y};
    return ab.x * ap.y - ab.y * ap.x;
}

// Função para rasterizar um triângulo com interpolação de cores
void FillTriangle(vec2_t v1, vec2_t v2, vec2_t v3, COLORREF *colors) {
    int x_min = floor(min(min(v1.x, v2.x), v3.x));
    int y_min = floor(min(min(v1.y, v2.y), v3.y));
    int x_max = ceil(max(max(v1.x, v2.x), v3.x));
    int y_max = ceil(max(max(v1.y, v2.y), v3.y));

    float delta_w0_col = v2.y - v3.y;
    float delta_w1_col = v3.y - v1.y;
    float delta_w2_col = v1.y - v2.y;

    float delta_w0_row = v3.x - v2.x;
    float delta_w1_row = v1.x - v3.x;
    float delta_w2_row = v2.x - v1.x;


    float area = EdgeCross(v1, v2, v3);

    float bias0 = IsTopLeft(v2, v3) ? 0 : -0.0001;
    float bias1 = IsTopLeft(v3, v1) ? 0 : -0.0001;
    float bias2 = IsTopLeft(v1, v2) ? 0 : -0.0001;

    vec2_t p0 = {x_min, y_min};
    float w0_row = EdgeCross(v2, v3, p0) + bias0;
    float w1_row = EdgeCross(v3, v1, p0) + bias1;
    float w2_row = EdgeCross(v1, v2, p0) + bias2;


    for (int y = y_min; y <= y_max; y++)
    {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++)
        {
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float alpha = w0 / (float)area;
                float beta  = w1 / (float)area;
                float gamma = w2 / (float)area;

                int r = alpha * GetRValue(colors[0]) + beta * GetRValue(colors[1]) + gamma * GetRValue(colors[2]);
                int g = alpha * GetGValue(colors[0]) + beta * GetGValue(colors[1]) + gamma * GetGValue(colors[2]);
                int b = alpha * GetBValue(colors[0]) + beta * GetBValue(colors[1]) + gamma * GetBValue(colors[2]);
                PutPixel((vec2_t){x, y}, RGB(r, g, b));
            }
            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }
        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}

void ShowScreen() {
    HDC hdc = GetDC(myWindow.hwnd);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = myWindow.screenWidth;
    bmi.bmiHeader.biHeight = -myWindow.screenHeight;  // Negativo para inverter a imagem
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;     // 3 bytes por pixel (RGB)
    bmi.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, myWindow.screenWidth, myWindow.screenHeight, 0, 0, myWindow.screenWidth, myWindow.screenHeight, myWindow.buffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(myWindow.hwnd, hdc);
}

void ClearScreen()
{
    memset(myWindow.buffer, 0, myWindow.screenWidth * myWindow.screenHeight * 3 * sizeof(BYTE));
}

// Função de callback para a janela
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            // Criar o buffer de imagem
            myWindow.buffer = (BYTE *)malloc(myWindow.screenWidth * myWindow.screenHeight * 3); // 3 bytes por pixel (RGB)
            if (myWindow.buffer == NULL) {
                MessageBox(NULL, (LPCSTR)"Erro ao alocar memória para o buffer!", (LPCSTR)"Erro", MB_OK | MB_ICONERROR);
                return 1;
            }

            ClearScreen();

            Init();

            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            free(myWindow.buffer);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int Start(unsigned int screenWidth, unsigned int screenHeight) {
    // Registrar a classe da janela
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = (LPCSTR)"MinhaJanelaClass";

    myWindow.screenWidth = screenWidth;
    myWindow.screenHeight = screenHeight;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, (LPCSTR)"Erro ao registrar a classe da janela!", (LPCSTR)"Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Criar a janela
    HWND hwnd = CreateWindowEx(0, (LPCSTR)"MinhaJanelaClass", (LPCSTR)"Minha Janela", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, myWindow.screenWidth, myWindow.screenHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
    myWindow.hwnd = hwnd;

    if (hwnd == NULL) {
        MessageBox(NULL, (LPCSTR)"Erro ao criar a janela!", (LPCSTR)"Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Mostrar e atualizar a janela
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    clock_t tp1, tp2;
    float elapsed_time;
    tp1 = clock();
    tp2 = clock();

    // Loop de mensagens
    MSG msg = {0};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else 
        {
            tp2 = clock();
            elapsed_time = ((float) (tp2 - tp1)) / CLOCKS_PER_SEC;
            tp1 = tp2;
            Update(elapsed_time);
        }        
    }

    return 0;
}

#endif