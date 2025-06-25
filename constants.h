#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <allegro5/allegro.h> // Para ALLEGRO_FONT, etc.
#include <allegro5/allegro_font.h> 

// Constantes da tela e elementos
const int SCREEN_W;
const int SCREEN_H;
const int GRASS_H;
const int NAVE_W;
const int NAVE_H;
const int ALIEN_W;
const int ALIEN_H;
const float FPS;

// Variáveis globais (declaradas aqui, definidas em tp.c ou main.c)
extern ALLEGRO_FONT *size_f;
extern ALLEGRO_BITMAP *alien_sprite;
extern ALLEGRO_BITMAP *alien2_sprite;
extern ALLEGRO_BITMAP *space_sprite;
extern ALLEGRO_BITMAP *space2_sprite;
extern ALLEGRO_BITMAP *earth_sprite;
extern ALLEGRO_BITMAP *nave_sprite;
extern ALLEGRO_BITMAP *nave2_sprite;

#endif // CONSTANTS_H