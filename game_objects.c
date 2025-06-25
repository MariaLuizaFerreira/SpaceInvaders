#include "game_objects.h"
#include "constants.h"
#include <stdio.h> 
#include <stdlib.h>


void initNave(Nave *nave){
    nave->x = SCREEN_W / 2;
    nave->vel = 5;
    nave->dir = 0;
    nave->esq = 0;
}

void initAlien(Alien *alien, int x, int y){
    alien->x = x + 30;
    alien->y = y;
    alien->vel_x = 1;
    alien->vel_y = ALIEN_H;
    alien->vida = 1;
    alien->ativo = 1;
}
void initAlienFase2(Alien *alien, int x, int y){
    alien->x = x;
    alien->y = y;
    alien->vel_x = 3;
    alien->vel_y = ALIEN_H;
    alien->vida = 10;
    alien->ativo = 1;
}

void initTiro(Tiro *tiro, int x){
    tiro->x = x;
    tiro->y = SCREEN_H - GRASS_H - 20;
    tiro->vel_y = 10;
}
void initMeteoro(Meteoro *meteoro, int x){
    meteoro->x = x;
    meteoro->y = -100; // Começa acima da tela
    meteoro->vel_y = 5;
    meteoro->ativo = 1;
}

void drawMeteoro(Meteoro meteoro, int raio){
    al_draw_filled_circle(meteoro.x, meteoro.y, raio, al_map_rgb(80, 84, 92));
}

void drawAlien(Alien alien){
    al_draw_bitmap(alien_sprite, alien.x, alien.y, 0);
}
void drawAlienFase2(Alien alien){
    al_draw_bitmap(alien2_sprite, alien.x, alien.y, 0);
    if(alien.vida > 0){
        al_draw_textf(size_f, al_map_rgb(255, 0, 0), alien.x + 20, alien.y + 60, 0, "%d", alien.vida);
    }

}

void draw_scenario(int score){
    al_draw_bitmap(space_sprite, 0, 0, 0);
    al_draw_bitmap(earth_sprite, 0, SCREEN_H - GRASS_H - 20, 0);
    
    char my_score[30];
    sprintf(my_score, "%d", score);
    al_draw_text(size_f, al_map_rgb(255, 255, 255), SCREEN_W - 925, SCREEN_H - 50, 0, my_score); 
}
void draw_scenario2(){
    al_draw_bitmap(space2_sprite, 0, 0, 0);
    //al_draw_bitmap(earth_sprite, 0, SCREEN_H - GRASS_H - 20, 0);
    
    //char my_score[30];
    //sprintf(my_score, "%d", score);
    //al_draw_text(size_f, al_map_rgb(255, 255, 255), SCREEN_W - 925, SCREEN_H - 50, 0, my_score); 
}
void draw_nave(Nave nave){
    float y_base = SCREEN_H - GRASS_H / 2;
    al_draw_bitmap(nave_sprite, nave.x - 64, y_base - NAVE_H, 0);
}
void draw_nave2(Nave nave){
    float y_base = SCREEN_H - GRASS_H / 2;
    al_draw_bitmap(nave2_sprite, nave.x - 64, y_base - NAVE_H, 0);
}

void drawTiro(Tiro tiro){
    al_draw_filled_circle(tiro.x, tiro.y, 6, al_map_rgb(245, 66, 120));
}

void update_nave(Nave *nave){
    if(nave->dir && nave->x + nave->vel <= SCREEN_W){
        nave->x += nave->vel;
    }
    if(nave->esq && nave->x - nave->vel >= 0){
        nave->x -= nave->vel;
    }
}

void updateAliens(Alien aliens[4][5]){
    int i, j;

    // verifica se algum alien da borda atingiu o limite da tela
    if(aliens[0][4].x + ALIEN_W + aliens[0][4].vel_x > SCREEN_W || aliens[0][0].x + aliens[0][0].vel_x < 0){
        for(j = 0; j < 4; j++){
            for(i = 0; i < 5; i++){
                aliens[j][i].y += aliens[j][i].vel_y;
                aliens[j][i].vel_x *= -1;
            }
        }
    }

    // movimenta todos os aliens
    for(j = 0; j < 4; j++){
        for(i = 0; i < 5; i++){
            aliens[j][i].x += aliens[j][i].vel_x;
        }
    }
}

void updateAliens2(Alien aliens[3]){
    int i;

    // verifica se algum alien da borda atingiu o limite da tela
    
    if(aliens[2].x + ALIEN_W + aliens[2].vel_x+25 > SCREEN_W || aliens[0].x + aliens[0].vel_x < 0){
        for(i = 0; i < 5; i++){
            aliens[i].y += aliens[i].vel_y;
            aliens[i].vel_x *= -1;
        }
        
    }

    // movimenta todos os aliens
    
    for(i = 0; i < 3; i++){
        aliens[i].x += aliens[i].vel_x;
     }
    
}