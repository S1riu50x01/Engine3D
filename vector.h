#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <math.h>

typedef struct{
    float x, y;
} vec2_t;

typedef struct{
	float x, y, z;
} vec3d;

typedef struct 
{
	float m[4][4];
} mat4x4;

void MatZero(mat4x4 *m)
{
    for (int i = 0; i < 4; i++)
     for (int j = 0; j < 4; j++)
        m->m[i][j] = 0;
}

void MultiplyMatrixVector(vec3d *i, vec3d *o, mat4x4 *m)
{
    o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
    o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
    o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
    float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];

    if (w != 0.0f)
    {
        o->x /= w; o->y /= w; o->z /= w;
    }
}


// Função para somar dois vetores
vec2_t add(vec2_t v1, vec2_t v2) {
    vec2_t result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

// Função para subtrair dois vetores
vec2_t subtract(vec2_t v1, vec2_t v2) {
    vec2_t result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

// Função para multiplicar um vetor por um escalar
vec2_t multiply(vec2_t v, float scalar) {
    vec2_t result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}

// Função para dividir um vetor por um escalar
vec2_t divide(vec2_t v, float scalar) {
    // Certifique-se de verificar se scalar não é zero para evitar divisão por zero
    if (scalar != 0.0f) {
        vec2_t result;
        result.x = v.x / scalar;
        result.y = v.y / scalar;
        return result;
    } else {
        // Retorna um vetor nulo para indicar uma operação inválida
        return (vec2_t){0.0f, 0.0f};
    }
}

// Função para calcular o produto escalar de dois vetores
float dotProduct(vec2_t v1, vec2_t v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

// Função para calcular o comprimento (magnitude) de um vetor
float length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

// Função para normalizar um vetor (torná-lo unitário)
vec2_t normalize(vec2_t v) {
    float len = length(v);
    // Certifique-se de verificar se o comprimento não é zero para evitar a divisão por zero
    if (len != 0.0f) 
    {
        vec2_t result;
        result.x = v.x / len;
        result.y = v.y / len;
        return result;
    }
    else return (vec2_t){0.0f, 0.0f};
}

vec2_t vec2_rotate(vec2_t v, vec2_t center, float angle) {
    vec2_t rot;
    v.x -= center.x;
    v.y -= center.y;
    rot.x = v.x * cos(angle) - v.y * sin(angle);
    rot.y = v.x * sin(angle) + v.y * cos(angle);
    rot.x += center.x;
    rot.y += center.y;
    return rot;
}

#endif