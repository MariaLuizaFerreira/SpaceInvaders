#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Para srand

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "constants.h"
#include "game_objects.h" // Inclui as novas declarações de funções


// Definição da variável de estado do jogo.
// Agora esta é a ÚNICA definição de 'current_state'.
GameState current_state = GAME_PHASE1; // Estado inicial do jogo

// Variáveis globais para sprites (precisam ser externas para game_objects.c)
ALLEGRO_BITMAP *alien_sprite = NULL;
ALLEGRO_BITMAP *space_sprite = NULL;
ALLEGRO_BITMAP *earth_sprite = NULL;
ALLEGRO_BITMAP *nave_sprite = NULL;
ALLEGRO_BITMAP *fase1_sprite = NULL;
ALLEGRO_BITMAP *fase2_sprite = NULL;
ALLEGRO_BITMAP *fase3_sprite = NULL; // Novo sprite para a transição da Fase 3
ALLEGRO_BITMAP *perdeu_sprite = NULL;

// Constantes globais (declaradas como extern em constants.h)
const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int GRASS_H = 60;
const int NAVE_W = 90;
const int NAVE_H = 96;
const int ALIEN_W = 50;
const int ALIEN_H = 25;
const float FPS = 60;
ALLEGRO_FONT *size_f;

int newRecord(float current_time, float *best_time_record) {
    FILE *arq = fopen("recorde.txt", "r");
    float stored_record = -1.0;

    if(arq != NULL) {
        fscanf(arq, "%f", &stored_record);
        fclose(arq);
    }

    if(stored_record == -1.0 || current_time < stored_record) {
        arq = fopen("recorde.txt", "w");
        if (arq != NULL) {
            fprintf(arq, "%.2f", current_time);
            fclose(arq);
        } else {
            fprintf(stderr, "Erro ao escrever no arquivo de recorde!\n");
        }
        *best_time_record = current_time;
        return 1;
    }
    *best_time_record = stored_record;
    return 0;
}


int colisao_alien_solo_ou_nave(Alien alien){
    if(current_state == GAME_PHASE1){
        if(alien.y + ALIEN_H > SCREEN_H - GRASS_H || alien.y + ALIEN_H >=  SCREEN_H - GRASS_H/2- NAVE_H){
            return 1;
        }
    }else if(current_state == GAME_PHASE2){
        // Aumentado o limiar de colisão para alien maior na fase 2
        if(alien.y + ALIEN_H + 14 > SCREEN_H - GRASS_H || alien.y + ALIEN_H + 14 >=  SCREEN_H - GRASS_H/2- NAVE_H){
            return 1;
        }
    }
    return 0;
}

int colisao_tiro_alien(Alien alien, Tiro tiro){
    if(current_state == GAME_PHASE1){
        if(tiro.x >= alien.x && tiro.x <= alien.x + ALIEN_W &&
        tiro.y >= alien.y && tiro.y <= alien.y + ALIEN_H){
        return 1;
        }
    }else if(current_state == GAME_PHASE2){
        // Aumentado o limiar de colisão para alien maior na fase 2
        if(tiro.x >= alien.x && tiro.x <= alien.x + ALIEN_W + 30 &&
        tiro.y >= alien.y && tiro.y <= alien.y + ALIEN_H){
            return 1;
        }
    }

    return 0;
}


int colisao_nave_meteoro(Nave nave, Meteoro meteoro, int raio) {
    // Centro da nave (ajustado para a nova posição da nave que é fixa na y)
    int centro_nave_x = nave.x + NAVE_W / 2;
    int centro_nave_y = (SCREEN_H - GRASS_H / 2) - (NAVE_H / 2);

    // Centro do meteoro (considerando meteoro.png 130x96)
    int centro_meteoro_x = meteoro.x + 130 / 2;
    int centro_meteoro_y = meteoro.y + 96 / 2;

    // Calcula distância entre centros
    int dx = centro_nave_x - centro_meteoro_x;
    int dy = centro_nave_y - centro_meteoro_y;
    float distancia = sqrt(dx * dx + dy * dy);

    // Raio da nave (aproximação segura)
    float raio_nave = 40;
    float soma_raios = raio + raio_nave;

    if (distancia <= soma_raios)
        return 1;
    return 0;
}

int main(int argc, char **argv){

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
	float time_elapsed = 0.0;
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    //inicializa o Allegro
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    if(!al_init_image_addon()) {
        fprintf(stderr, "failed to initialize image addon!\n");
        return -1;
    }
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
        fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

    //cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
        fprintf(stderr, "failed to create display!\n");
        return -1;
    }
    // Carrega sprites (alguns podem ser sobrescritos nas transições de fase)
    alien_sprite = al_load_bitmap("assets/alien.png");
    if (!alien_sprite) { fprintf(stderr, "failed to load alien.png!\n"); al_destroy_display(display); return -1; }
    space_sprite = al_load_bitmap("assets/space.jpeg");
    if (!space_sprite) { fprintf(stderr, "failed to load space.jpeg!\n"); al_destroy_bitmap(alien_sprite); al_destroy_display(display); return -1; }
    earth_sprite = al_load_bitmap("assets/earth.png");
    if (!earth_sprite) { fprintf(stderr, "failed to load earth.png!\n"); al_destroy_bitmap(alien_sprite); al_destroy_bitmap(space_sprite); al_destroy_display(display); return -1; }
    nave_sprite = al_load_bitmap("assets/nave.png");
    if (!nave_sprite) { fprintf(stderr, "failed to load nave.png!\n"); al_destroy_bitmap(alien_sprite); al_destroy_bitmap(space_sprite); al_destroy_bitmap(earth_sprite); al_destroy_display(display); return -1; }
    fase1_sprite = al_load_bitmap("assets/fase1.png");
    if (!fase1_sprite) { fprintf(stderr, "failed to load fase1.png!\n"); /* cleanup */ return -1; }
    fase2_sprite = al_load_bitmap("assets/fase2.png");
    if (!fase2_sprite) { fprintf(stderr, "failed to load fase2.png!\n"); /* cleanup */ return -1; }
    fase3_sprite = al_load_bitmap("assets/fase3.png"); // Carrega o novo sprite para a Fase 3
    if (!fase3_sprite) { fprintf(stderr, "failed to load fase3.png!\n"); /* cleanup */ return -1; }
    perdeu_sprite = al_load_bitmap("assets/perdeu.png");
    if (!perdeu_sprite) { fprintf(stderr, "failed to load perdeu.png!\n"); /* cleanup */ return -1; }


    //cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }
    //inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
    //inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();

    //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    size_f = al_load_font("arial.ttf", 32, 1);
    //cria a fila de eventos
    event_queue = al_create_event_queue();
    if(!event_queue) {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        return -1;
    }

    //instala o teclado
    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to install keyboard!\n");
        return -1;
    }

    //instala o mouse
    if(!al_install_mouse()) {
        fprintf(stderr, "failed to initialize mouse!\n");
        return -1;
    }


    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    Nave nave;
    initNave(&nave);

    Tiro tiro;
    int temTiro = 0;

    Tiro tiros_fase2[3];
    int temTiro_fase2[3] = {0, 0, 0};

    Meteoro meteoro;
    int temMeteoro = 0, raio = 0;

    struct AlienBoss alienMaster; // Alien chefe para a Fase 3 (tipo struct AlienBoss)
    PowerUp power_up; // Power-up para a Fase 3
    int temPowerUp = 0;
    double last_powerup_drop_time = 0; // Para controlar o tempo de queda dos power-ups

    Alien aliens[4][5];

    int i;
    int j;
    for(j=0;j<4;j++){
        int pos_y = j * (ALIEN_H + 30);
        for(i = 0; i<5; i++){
            int pos_x = i * (ALIEN_W + 30);
            initAlien(&aliens[j][i], pos_x, pos_y);
        }
    }

    int pontos = 0;
    int playing = 1;
    int ganhou = 0, perdeu = 0;

    // Tela de início da Fase 1
    al_draw_bitmap(fase1_sprite, 0, 0, 0);
    al_flip_display();
    al_rest(2);
    al_start_timer(timer);

    Alien aliens_fase2[3];
    int num_aliens_fase2_ativos = 0; // Controla quantos aliens estão ativos na Fase 2


    while (current_state != GAME_OVER_WIN && current_state != GAME_OVER_LOSE && playing)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if (current_state == GAME_PHASE1) {
                draw_scenario(pontos);
                update_nave(&nave);
                updateAliens(aliens);

                // Verifica se todos os aliens da Fase 1 foram destruídos
                int todos_aliens_destruidos = 1;
                for(j=0;j<4;j++){
                    for(i=0;i<5;i++){
                        if(aliens[j][i].ativo == 1){
                            todos_aliens_destruidos = 0;
                            break;
                        }
                    }
                    if(!todos_aliens_destruidos) break;
                }

                if(todos_aliens_destruidos){ // Transição para a Fase 2
                    current_state = GAME_PHASE2;
                    // Inicializa os aliens da Fase 2
                    for(i = 0; i < 3; i++){
                        initAlienFase2(&aliens_fase2[i], i * (ALIEN_W + 100)+270, ALIEN_H + 30);
                    }
                    num_aliens_fase2_ativos = 3; // Define o número inicial de aliens na Fase 2

                    // Carrega novos sprites para a Fase 2
                    al_destroy_bitmap(space_sprite);
                    space_sprite = al_load_bitmap("assets/fase2_espaco.jpg");
                    al_destroy_bitmap(alien_sprite);
                    alien_sprite = al_load_bitmap("assets/alien2.png");
                    al_destroy_bitmap(nave_sprite);
                    nave_sprite = al_load_bitmap("assets/nave2.png");

					time_elapsed = al_get_time();
                    al_stop_timer(timer);
                    initNave(&nave); // Reinicia a nave para o centro
                    al_draw_bitmap(fase2_sprite, 0, 0, 0); // Tela de transição
                    al_flip_display();
                    al_rest(2);
                    al_start_timer(timer);
                }

                draw_nave(nave);
                for(j=0;j<4;j++){
                    for(i = 0; i<5; i++){
                        if(aliens[j][i].ativo == 1){
                            drawAlien(aliens[j][i]);
                        }
                    }
                }

                if (temTiro) {
                    tiro.y -= tiro.vel_y;
                    drawTiro(tiro);

                    if (tiro.y < 0) {
                        temTiro = 0;
                    }
                }

                // Colisão alien-solo ou nave
                for(j=0;j<4;j++){
                    for(i = 0; i<5; i++){
                        if(aliens[j][i].ativo == 1 && colisao_alien_solo_ou_nave(aliens[j][i])){
                            playing = 0; // Fim de jogo
                            perdeu = 1;
                            break;
                        }
                    }
                    if(!playing) break;
                }

                // Colisão tiro-alien
                for(j=0;j<4;j++){
                    for(i = 0; i<5; i++){
                        if(aliens[j][i].ativo == 1 && temTiro && colisao_tiro_alien(aliens[j][i], tiro)){
                            temTiro = 0;
                            aliens[j][i].ativo = 0;
                            pontos++; // Incrementa pontos por alien destruído
                            break;
                        }
                    }
                    if (!temTiro) {
                        break;
                    }
                }
            } else if (current_state == GAME_PHASE2) {
                draw_scenario2(); // Cenário da Fase 2
                update_nave(&nave);

                // Verifica se todos os aliens da Fase 2 foram destruídos
                if(num_aliens_fase2_ativos == 0) { // Transição para a Fase 3
                    current_state = GAME_PHASE3;
                    initAlienFase3(&alienMaster); // Inicializa o alien chefe
                    last_powerup_drop_time = al_get_time(); // Inicia o contador de power-ups

                    // Carrega novos sprites para a Fase 3
                    al_destroy_bitmap(alien_sprite);
                    alien_sprite = al_load_bitmap("assets/alien3.png"); // Sprite do chefe
                    if (!alien_sprite) { fprintf(stderr, "failed to load alien3.png!\n"); /* cleanup */ playing = 0; perdeu = 1;}

                    al_destroy_bitmap(space_sprite);
                    space_sprite = al_load_bitmap("assets/espaco3.jpg"); // Novo fundo
                    if (!space_sprite) { fprintf(stderr, "failed to load espaco3.jpg!\n"); /* cleanup */ playing = 0; perdeu = 1;}

                    al_destroy_bitmap(nave_sprite);
                    nave_sprite = al_load_bitmap("assets/nave3.png"); // Nova nave
                    if (!nave_sprite) { fprintf(stderr, "failed to load nave3.png!\n"); /* cleanup */ playing = 0; perdeu = 1;}

					time_elapsed += al_get_time();
                    al_stop_timer(timer);
                    initNave(&nave); // Reinicia a nave para o centro
                    al_draw_bitmap(fase3_sprite, 0, 0, 0); // Tela de transição
                    al_flip_display();
                    al_rest(2);
                    al_start_timer(timer);
                }

                // Desenha e atualiza aliens da Fase 2
                for(i = 0; i<3; i++){
                    if(aliens_fase2[i].ativo == 1){
                        drawAlienFase2(aliens_fase2[i]);
                    }
                }
                updateAliens2(aliens_fase2); // Atualiza aliens da Fase 2

                // Queda de meteoros na Fase 2
                if(al_get_timer_count(timer) % (int)(FPS * 3) == 0){ // A cada 3 segundos, um meteoro
                    initMeteoro(&meteoro, rand() % (SCREEN_W - 20) + 10);
                    temMeteoro = 1;
                    raio = 10 + rand()%21; // Raio aleatório
                }
                if (temMeteoro) {
                    meteoro.y += meteoro.vel_y;
                    drawMeteoro(meteoro, raio);

                    if (meteoro.y > SCREEN_H) {
                        temMeteoro = 0;
                    }
                }

                // Tiros da nave na Fase 2 (múltiplos tiros)
                for (i = 0; i < 3; i++) {
                    if (temTiro_fase2[i]) {
                        tiros_fase2[i].y -= tiros_fase2[i].vel_y;
                        drawTiro(tiros_fase2[i]);

                        if (tiros_fase2[i].y < 0) {
                            temTiro_fase2[i] = 0;
                        }
                    }
                }

                // Colisão tiro-alien (Fase 2)
                for (i = 0; i < 3; i++) {
                    if (temTiro_fase2[i]) {
                        int j;
                        for (j = 0; j < 3; j++) {
                            if (aliens_fase2[j].ativo == 1 && colisao_tiro_alien(aliens_fase2[j], tiros_fase2[i])) {
                                temTiro_fase2[i] = 0;
                                aliens_fase2[j].vida--;
                                if(aliens_fase2[j].vida <= 0) {
                                    aliens_fase2[j].ativo = 0;
                                    num_aliens_fase2_ativos--; // Decrementa aliens ativos
                                }
                                pontos++;
                                break;
                            }
                        }
                    }
                }

                // Colisão nave-meteoro (Fase 2)
                if(colisao_nave_meteoro(nave, meteoro, raio)){
                    playing = 0;
                    perdeu = 1;
                }

                // Colisão alien-solo ou nave (Fase 2)
                for(i = 0; i<3; i++){
                    if(aliens_fase2[i].ativo == 1 && colisao_alien_solo_ou_nave(aliens_fase2[i])){
                        playing = 0; // Fim de jogo
                        perdeu = 1;
                        break;
                    }
                }
                draw_nave2(nave); // Desenha a nave da Fase 2

            } else if(current_state == GAME_PHASE3) {
                draw_scenario3(alienMaster.vida); // Desenha o cenário da Fase 3 com a vida do chefe
                update_nave(&nave); // Atualiza a nave
                draw_nave2(nave); // Reutilizando a função de desenho da nave da fase 2

                // Atualiza e desenha o alien chefe
                updateAlienFase3(&alienMaster);
                drawAlienFase3(alienMaster);

                // Lógica para queda de power-ups a cada 10 segundos
                if (al_get_time() - last_powerup_drop_time >= POWERUP_DROP_INTERVAL) {
                    if (!temPowerUp) { // Só permite um power-up ativo por vez
                        // Gera power-up em uma posição x aleatória acima da tela
                        initPowerUp(&power_up, rand() % (SCREEN_W - 50) + 25, -30);
                        temPowerUp = 1;
                        last_powerup_drop_time = al_get_time(); // Reseta o timer
                    }
                }

                // Atualiza e desenha o power-up
                if (temPowerUp) {
                    updatePowerUp(&power_up);
                    drawPowerUp(power_up);

                    // Verifica colisão da nave com o power-up
                    if (colisao_nave_powerup(nave, power_up)) {
                        temPowerUp = 0; // Desativa o power-up após colisão
                        alienMaster.vida -= POWERUP_DAMAGE; // Causa dano ao chefe
                        // Verificar vitória após power-up
                        if (alienMaster.vida <= 0) {
                            alienMaster.ativo = 0; // Desativa o chefe
                            current_state = GAME_OVER_WIN; // Venceu o jogo!
                            ganhou = 1;
							time_elapsed += al_get_time(); // Adiciona o tempo do jogo
                        }
                    }
                }

                // Lógica para tiro da nave na Fase 3
                if (temTiro) { // Se há um tiro ativo
                    tiro.y -= tiro.vel_y; // Move o tiro
                    drawTiro(tiro); // Desenha o tiro

                    // Colisão do tiro com o Alien Chefe
                    if (alienMaster.ativo && colisao_tiro_alienboss(tiro, alienMaster)) {
                        alienMaster.vida -= tiro.damage; // Aplica o dano do tiro
                        temTiro = 0; // Desativa o tiro após colisão

                        // Verificar vitória após tiro
                        if (alienMaster.vida <= 0) {
                            alienMaster.ativo = 0; // Desativa o chefe
                            current_state = GAME_OVER_WIN; // Venceu o jogo!
                            ganhou = 1;
                        }
                    } else if (tiro.y < 0) { // Se o tiro sair da tela
                        temTiro = 0; // Desativa o tiro
                    }
                }


                // Verifica se o chefe tocou o solo (se updateAlienFase3 não já mudou o estado)
                // Isso é uma redundância para garantir que o jogo termina se o chefe descer demais
                if (alienMaster.ativo && alienMaster.y + (ALIEN_H + 100) > SCREEN_H - GRASS_H) {
                    playing = 0;
                    perdeu = 1;
                }

            }

            //atualiza a tela
            al_flip_display();

            if(al_get_timer_count(timer) % (int)FPS == 0)
                printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            playing = 0;
        }
        else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
        }
        // Se o tipo de evento for um pressionar de uma tecla
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            printf("\ncodigo tecla: %d", ev.keyboard.keycode);

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_A:
                nave.esq = 1;
                break;
            case ALLEGRO_KEY_D:
                nave.dir = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                if (current_state == GAME_PHASE1) {
                    if (!temTiro) {
                        initTiro(&tiro, nave.x, SHIP_SHOT_DAMAGE_PHASE1); // MODIFICADO: Passa o dano
                        temTiro = 1;
                    }
                } else if (current_state == GAME_PHASE2) {
                    for (i = 0; i < 3; i++) {
                        if (!temTiro_fase2[i]) {
                            initTiro(&tiros_fase2[i], nave.x, SHIP_SHOT_DAMAGE_PHASE2); // MODIFICADO: Passa o dano
                            temTiro_fase2[i] = 1;
                            break;
                        }
                    }
                } else if (current_state == GAME_PHASE3) { // NOVO: Lógica de tiro para a Fase 3
                    if (!temTiro) { // Permite apenas um tiro na tela por vez para o boss
                        initTiro(&tiro, nave.x, SHIP_SHOT_DAMAGE_PHASE3); // MODIFICADO: Passa o dano para o tiro do boss
                        temTiro = 1;
                    }
                }
                break;
            default:
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
            printf("\ncodigo tecla: %d", ev.keyboard.keycode);

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_A:
                nave.esq = 0;
                break;
            case ALLEGRO_KEY_D:
                nave.dir = 0;
                break;

            default:
                break;
            }
        }
    }

    // Telas de Fim de Jogo (Vitória/Derrota)
    if(ganhou){
        float record;
        al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto para tela de vitória
        al_draw_text(size_f, al_map_rgb(0, 255, 0), SCREEN_W/2, SCREEN_H/2 - 50, ALLEGRO_ALIGN_CENTER, "VOCE VENCEU!");
        char my_score[20];
        char time[50]; // Aumentar buffer para a string do tempo

        sprintf(my_score, "Score: %d", pontos);
        al_draw_text(size_f, al_map_rgb(200, 0, 30), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, my_score);

        if(newRecord(time_elapsed, &record)) {
            al_draw_text(size_f, al_map_rgb(200, 20, 30), SCREEN_W/2, SCREEN_H/2 + 50, ALLEGRO_ALIGN_CENTER, "NOVO RECORDE DE TEMPO!");
            sprintf(time, "Seu tempo: %.2f segundos",time_elapsed );
            al_draw_text(size_f, al_map_rgb(200, 0, 30), SCREEN_W/2, SCREEN_H/2 + 100, ALLEGRO_ALIGN_CENTER, time);
        }
        else {
            sprintf(time, "Recorde: %.2f segundos", record);
            al_draw_text(size_f, al_map_rgb(0, 200, 30), SCREEN_W/2, SCREEN_H/2 + 100, ALLEGRO_ALIGN_CENTER, time);
        }
        al_flip_display();
        al_rest(7);
    }

    if(perdeu){
        // A imagem de "perdeu" já é carregada no início
        al_draw_bitmap(perdeu_sprite, 0, 0, 0);
        al_flip_display();
        al_rest(7);
    }

    // Destruição de recursos do Allegro
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(alien_sprite);
    al_destroy_bitmap(space_sprite);
    al_destroy_bitmap(earth_sprite);
    al_destroy_bitmap(nave_sprite);
    al_destroy_bitmap(perdeu_sprite);
    al_destroy_bitmap(fase1_sprite);
    al_destroy_bitmap(fase2_sprite);
    al_destroy_bitmap(fase3_sprite); // Destrói o novo sprite da Fase 3
    al_destroy_font(size_f);

    return 0;
}