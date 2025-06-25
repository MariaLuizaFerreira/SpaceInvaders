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
    float damage; 
    int ativo; // 1 se ativo, 0 se não
} Tiro;

typedef struct Meteoro
{
    float x, y;
    float vel_y;
    int ativo;
} Meteoro;
#endif // STRUCTS_H
typedef struct PowerUp {
    float x, y;
    float vel_y;
    int ativo;
} PowerUp;


typedef struct AlienBoss {
    float x, y;
    int vida;
    float vel_y; 
    int ativo;
} AlienBoss;