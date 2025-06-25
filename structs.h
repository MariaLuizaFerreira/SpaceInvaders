#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Nave
{
    float x;
    float vel;
    int dir, esq;
} Nave;

typedef struct Alien
{
    float x, y;
    float vel_x, vel_y;
    int vida;
    int ativo;
} Alien;

typedef struct Tiro
{
    float x, y;
    float vel_y;
} Tiro;

typedef struct Meteoro
{
    float x, y;
    float vel_y;
    int ativo;
} Meteoro;
#endif // STRUCTS_H