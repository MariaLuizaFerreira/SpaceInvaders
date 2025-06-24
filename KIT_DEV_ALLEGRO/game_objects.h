#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include "structs.h" // Inclui as definições das structs
#include "constants.h"
#include <allegro5/allegro_primitives.h> // Para al_draw_filled_circle
#include <allegro5/allegro_font.h>       // Para al_draw_text

// Funções de inicialização
void initNave(Nave *nave);
void initAlien(Alien *alien, int x, int y);
void initTiro(Tiro *tiro, int x);

// Funções de desenho
void drawAlien(Alien alien);
void draw_scenario(int score);
void draw_nave(Nave nave);
void drawTiro(Tiro tiro);
void drawMeteoro(Meteoro meteoro, int raio);
void drawAlienFase2(Alien alien);

// Funções de atualização
void update_nave(Nave *nave);
void updateAliens(Alien aliens[4][5]);
void updateAliens2(Alien aliens[3]);

#endif // GAME_OBJECTS_H