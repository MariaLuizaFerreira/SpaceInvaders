#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <allegro5/allegro.h> // Para ALLEGRO_FONT, etc.
#include <allegro5/allegro_font.h> 

#define BOSS_HEALTH 100
#define POWERUP_DAMAGE 10
#define POWERUP_DROP_INTERVAL 10.0 // Segundos entre a queda de power-ups
#define BOSS_SPEED 0.2           // Velocidade de movimento do boss no eixo Y
#define POWERUP_SPEED 3.0   
// Dano dos tiros da nave por fase
#define SHIP_SHOT_DAMAGE_PHASE1 1 // Dano padrão dos tiros na Fase 1
#define SHIP_SHOT_DAMAGE_PHASE2 1 // Dano padrão dos tiros na Fase 2
#define SHIP_SHOT_DAMAGE_PHASE3 2
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
extern ALLEGRO_BITMAP *space_sprite;
extern ALLEGRO_BITMAP *earth_sprite;
extern ALLEGRO_BITMAP *nave_sprite;
extern ALLEGRO_BITMAP *fase1_sprite;
extern ALLEGRO_BITMAP *fase2_sprite;
extern ALLEGRO_BITMAP *perdeu_sprite;

typedef enum {
    GAME_PHASE1,
    GAME_PHASE2,
    GAME_PHASE3,
    GAME_OVER_WIN,
    GAME_OVER_LOSE
} GameState;

extern GameState current_state;

#endif // CONSTANTS_H