#include "game_objects.h"
//#include "constants.h"
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



void initTiro(Tiro *tiro, float nave_x, int damage_val) {
    tiro->x = nave_x + NAVE_W / 2 - 35; // Centraliza o tiro
    tiro->y = (SCREEN_H - GRASS_H / 2) - NAVE_H - 10; // Acima da nave
    tiro->vel_y = 10.0;
    tiro->ativo = 1;
    tiro->damage = damage_val; // Atribui o valor de dano
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
    al_draw_bitmap(alien_sprite, alien.x, alien.y, 0);
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
    al_draw_bitmap(space_sprite, 0, 0, 0);
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
    al_draw_bitmap(nave_sprite, nave.x - 64, y_base - NAVE_H, 0);
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
void initAlienFase3(AlienBoss *boss) {
    // Posição inicial centralizada na parte superior
    boss->x = SCREEN_W / 2 - (ALIEN_W + 100) / 2; // Ajuste ALIEN_W+100 para o tamanho do sprite do chefe
    boss->y = 0;
    boss->vida = BOSS_HEALTH;
    boss->vel_y = BOSS_SPEED;
    boss->ativo = 1;
}

void drawAlienFase3(AlienBoss boss) {
    if (boss.ativo) {
        al_draw_bitmap(alien_sprite, boss.x, boss.y, 0);

        float health_bar_width = (float)boss.vida / BOSS_HEALTH * 150; // Largura máxima da barra (ex: 150 pixels)
        // Fundo da barra de vida (vermelho escuro)
        al_draw_filled_rectangle(boss.x, boss.y - 20, boss.x + 150, boss.y - 10, al_map_rgb(100, 0, 0));
        // Preenchimento da barra de vida (verde)
        al_draw_filled_rectangle(boss.x, boss.y - 20, boss.x + health_bar_width, boss.y - 10, al_map_rgb(0, 255, 0));
        // Borda da barra de vida (branca)
        al_draw_rectangle(boss.x, boss.y - 20, boss.x + 150, boss.y - 10, al_map_rgb(255, 255, 255), 1);

        // Opcional: Desenhar a vida em número
        al_draw_textf(size_f, al_map_rgb(255, 255, 255), boss.x + 75, boss.y - 35, ALLEGRO_ALIGN_CENTER, "%d/%d", boss.vida, BOSS_HEALTH);
    }
}

void updateAlienFase3(AlienBoss *boss) {
    if (boss->ativo) {
        boss->y += boss->vel_y;

        // Limite para que o chefe não saia da tela por baixo
        // ALIEN_H + 100 é um valor aproximado para o tamanho do sprite do chefe
        // Você pode ajustar isso ou criar uma nova constante BOSS_H
        if (boss->y + ALIEN_H + 100 > SCREEN_H - GRASS_H) {
            boss->y = SCREEN_H - GRASS_H - (ALIEN_H + 100); // Reposiciona na borda do "solo"
            // Se o chefe tocar o solo, o jogador perde
            current_state = GAME_OVER_LOSE;
        }
    }
}


// --- Funções para PowerUps ---

void initPowerUp(PowerUp *pu, float x, float y) {
    pu->x = x;
    pu->y = y;
    pu->vel_y = POWERUP_SPEED;
    pu->ativo = 1;
}

void drawPowerUp(PowerUp pu) {
    if (pu.ativo) {
        // Desenha um quadrado amarelo como power-up
        al_draw_filled_rectangle(pu.x, pu.y, pu.x + 20, pu.y + 20, al_map_rgb(255, 255, 0));
        // Opcional: Adicionar um ícone ou sprite para o power-up
        // al_draw_bitmap(powerup_sprite, pu.x, pu.y, 0);
    }
}

void updatePowerUp(PowerUp *pu) {
    if (pu->ativo) {
        pu->y += pu->vel_y;
        // Se o power-up sair da tela por baixo, desativa
        if (pu->y > SCREEN_H) {
            pu->ativo = 0;
        }
    }
}

int colisao_nave_powerup(Nave nave, PowerUp power_up) {
    if (!power_up.ativo) return 0;

    // Assumindo a nave está sempre na parte inferior da tela com y = (SCREEN_H - GRASS_H / 2) - NAVE_H
    // Nave bounding box: (nave.x, (SCREEN_H - GRASS_H / 2) - NAVE_H) a (nave.x + NAVE_W, (SCREEN_H - GRASS_H / 2))
    // PowerUp bounding box: (power_up.x, power_up.y) a (power_up.x + 20, power_up.y + 20) (assumindo tamanho 20x20)

    if (nave.x < power_up.x + 20 &&
        nave.x + NAVE_W > power_up.x &&
        (SCREEN_H - GRASS_H / 2) - NAVE_H < power_up.y + 20 &&
        (SCREEN_H - GRASS_H / 2) > power_up.y) {
        return 1;
    }
    return 0;
}

int colisao_tiro_alienboss(Tiro tiro, AlienBoss boss) {
    // Tiro bounding box: (tiro.x, tiro.y) a (tiro.x + 10, tiro.y + 20) (assumindo tamanho do tiro 10x20)
    // Boss bounding box: (boss.x, boss.y) a (boss.x + (ALIEN_W + 100), boss.y + (ALIEN_H + 100))

    if (!tiro.ativo || !boss.ativo) return 0;

    // Colisão AABB (Axis-Aligned Bounding Box)
    if (tiro.x < boss.x + (ALIEN_W + 100) &&
        tiro.x + 10 > boss.x && // 10 é a largura do tiro
        tiro.y < boss.y + (ALIEN_H + 100) &&
        tiro.y + 20 > boss.y) { // 20 é a altura do tiro
        return 1;
    }
    return 0;
}

void draw_scenario3(int boss_health) {
    al_draw_bitmap(space_sprite, 0, 0, 0); // Fundo
    al_draw_bitmap(earth_sprite, 0, SCREEN_H - GRASS_H, 0); // Terra

    // Exibe a vida do chefe no canto superior direito
    al_draw_textf(size_f, al_map_rgb(255, 255, 255), SCREEN_W - 10, 10, ALLEGRO_ALIGN_RIGHT, "Boss HP: %d", boss_health);
}
