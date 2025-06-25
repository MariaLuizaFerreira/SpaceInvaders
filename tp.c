#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "constants.h"
#include "structs.h"
#include "game_objects.h"

typedef enum {
    GAME_PHASE1,
    GAME_PHASE2,
	GAME_PHASE3, // Adicionando uma terceira fase, se necessário
    GAME_OVER_WIN,
    GAME_OVER_LOSE
} GameState;

GameState current_state = GAME_PHASE1;

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int GRASS_H = 60;

const int NAVE_W = 90;
const int NAVE_H = 96;

const int ALIEN_W = 50;
const int ALIEN_H = 25;

const float FPS = 60;  
ALLEGRO_FONT *size_f;

ALLEGRO_BITMAP *alien_sprite = NULL;
ALLEGRO_BITMAP *alien2_sprite = NULL;
ALLEGRO_BITMAP *space_sprite = NULL;
ALLEGRO_BITMAP *space2_sprite = NULL;
ALLEGRO_BITMAP *earth_sprite = NULL;
ALLEGRO_BITMAP *nave_sprite = NULL;
ALLEGRO_BITMAP *nave2_sprite = NULL;
ALLEGRO_BITMAP *perdeu_sprite = NULL;
ALLEGRO_BITMAP *fase1_sprite = NULL;
ALLEGRO_BITMAP *fase2_sprite = NULL;

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
		if(alien.y + ALIEN_H + 14> SCREEN_H - GRASS_H || alien.y + ALIEN_H +14>=  SCREEN_H - GRASS_H/2- NAVE_H){
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
		if(tiro.x >= alien.x && tiro.x <= alien.x + ALIEN_W +30 &&
		tiro.y >= alien.y && tiro.y <= alien.y + ALIEN_H){
			return 1;
		}
	}
	
	return 0;
}



int colisao_nave_meteoro(Nave nave, Meteoro meteoro, int raio) {
    // Centro da nave
    int centro_nave_x = nave.x;
    int centro_nave_y = (SCREEN_H - GRASS_H / 2) - (NAVE_H / 2);

    // Centro do meteoro
    int centro_meteoro_x = meteoro.x + 130 / 2;
    int centro_meteoro_y = meteoro.y + 96 / 2;

    // Calcula distância entre centros
    int dx = centro_nave_x - centro_meteoro_x;
    int dy = centro_nave_y - centro_meteoro_y;
    float distancia = sqrt(dx * dx + dy * dy);

    // Raio da nave (ajuste conforme o seu desenho visual)
    float raio_nave = 40;  // Aproximação segura para sua nave
    float soma_raios = raio + raio_nave;

    if (distancia <= soma_raios)
        return 1;
    return 0;
}

int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	
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
	alien_sprite = al_load_bitmap("assets/alien.png"); // Carrega a imagem
	if (!alien_sprite) {
		fprintf(stderr, "failed to load alien.png!\n");
		al_destroy_display(display);
		return -1;
	}
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

	space_sprite = al_load_bitmap("assets/space.jpeg"); // Carrega a imagem de fundo
	space2_sprite = al_load_bitmap("assets/fase2_espaco.jpg"); // Carrega a imagem de fundo
	earth_sprite = al_load_bitmap("assets/earth.png"); // Carrega a imagem da terra
	Nave nave;
	nave_sprite = al_load_bitmap("assets/nave.png"); // Carrega a imagem da nave
	initNave(&nave);

	Nave nave2;
	Tiro tiro;
	int temTiro = 0;

	Tiro tiros_fase2[3];
	int temTiro_fase2[3] = {0, 0, 0};

	Meteoro meteoro;
	int temMeteoro = 0, raio = 0;

	Alien alien;
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
	//inicia o temporizador
	fase1_sprite = al_load_bitmap("assets/fase1.png");
	fase2_sprite = al_load_bitmap("assets/fase2.png");
	al_draw_bitmap(fase1_sprite, 0, 0, 0);
	al_flip_display();	
	al_rest(2);
	al_start_timer(timer);

	Alien aliens_fase2[3]; // Ajuste o tamanho conforme o número de aliens na Fase 2
	int num_aliens_fase2_ativos = 0;

	while (current_state != GAME_OVER_WIN && current_state != GAME_OVER_LOSE && playing)
	{

		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			if (current_state == GAME_PHASE1) {
				draw_scenario(pontos);
				update_nave(&nave);
				updateAliens(aliens);
				if(pontos == 20){
					current_state = GAME_PHASE2;
					for(i = 0; i < 3; i++){
						// As coordenadas e tamanhos dos aliens da Fase 2 podem ser diferentes
						initAlienFase2(&aliens_fase2[i], i * (ALIEN_W + 100)+270, ALIEN_H + 30);
					}
					num_aliens_fase2_ativos = 3; // Define o número inicial de aliens na Fase 2
					printf("Entrando na Fase 2!\n"); // Debugging
					alien2_sprite = al_load_bitmap("assets/alien2.png");
					nave2_sprite = al_load_bitmap("assets/nave2.png");
					al_stop_timer(timer);
					initNave(&nave2);
					al_draw_bitmap(fase2_sprite, 0, 0, 0);
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
			
					// Se o tiro sair da tela, desativa
					if (tiro.y < 0) {
						temTiro = 0;
					}
				}

				for(j=0;j<4;j++){
					for(i = 0; i<5; i++){
						if(aliens[j][i].ativo == 1 && colisao_alien_solo_ou_nave(aliens[j][i])){
							playing = !colisao_alien_solo_ou_nave(aliens[j][i]);
							perdeu = 1;
							break; 
						}	
					}
					if(!playing){
						break;
					}
				}


				for(j=0;j<4;j++){
					for(i = 0; i<5; i++){
						if(aliens[j][i].ativo == 1 && temTiro && colisao_tiro_alien(aliens[j][i], tiro)){
							temTiro = 0; 
							aliens[j][i].ativo = 0; 
							pontos++;
							break; 
						}
					}
					if (!temTiro) { 
						break;
					}
				}
    		} else if (current_state == GAME_PHASE2) {
				draw_scenario2();
				update_nave(&nave2);
				if(num_aliens_fase2_ativos == 0) {
					ganhou = 1;
					playing = 0;
				}
				for(i = 0; i<3; i++){
					if(aliens_fase2[i].ativo == 1){
						drawAlienFase2(aliens_fase2[i]);
					}
				}
				updateAliens2(aliens_fase2);
				if(al_get_timer_count(timer) % 200 == 0){
					initMeteoro(&meteoro, rand() % (SCREEN_W - 20) + 10);
					temMeteoro = 1;
					raio = 10 + rand()%21;
				}
				if (temMeteoro) {
					meteoro.y += meteoro.vel_y;
					drawMeteoro(meteoro, raio);
			
					// Se o meteoro sair da tela, desativa
					if (meteoro.y > SCREEN_H) {
						temMeteoro = 0;
					}
				}
				for (i = 0; i < 3; i++) {
					if (temTiro_fase2[i]) {
						tiros_fase2[i].y -= tiros_fase2[i].vel_y;
						drawTiro(tiros_fase2[i]);

						// Se o tiro sair da tela, desativa
						if (tiros_fase2[i].y < 0) {
							temTiro_fase2[i] = 0;
						}
					}
				}
				// Verifica colisão de cada tiro com cada alien ativo
				for (i = 0; i < 3; i++) {
					if (temTiro_fase2[i]) {
						int j;
						for (j = 0; j < 3; j++) {
							if (aliens_fase2[j].ativo == 1 && colisao_tiro_alien(aliens_fase2[j], tiros_fase2[i])) {
								temTiro_fase2[i] = 0; 
								if(aliens_fase2[j].vida <= 1) {
									aliens_fase2[j].ativo = 0; // Desativa o alien se a vida chegar a zero
									num_aliens_fase2_ativos--;
								}
								aliens_fase2[j].vida--;
								pontos++;
								break;
							}
						}
					}
				}

				if(colisao_nave_meteoro(nave2, meteoro, raio)){
					playing = 0;
					perdeu = 1;
				}

				
				for(i = 0; i<3; i++){
					if(aliens_fase2[i].ativo == 1 && colisao_alien_solo_ou_nave(aliens_fase2[i])){
						playing = !colisao_alien_solo_ou_nave(aliens_fase2[i]);
						perdeu = 1;
						break; 
					}	
				}
				draw_nave2(nave2);
				
			}
			
			
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();

			if(al_get_timer_count(timer) % (int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}//se o tipo de evento for um clique de mouse
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { 
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_A:
				if(current_state == GAME_PHASE1)
					nave.esq = 1;
				else if(current_state == GAME_PHASE2)
					nave2.esq = 1;
				break;
			case ALLEGRO_KEY_D:
				if(current_state == GAME_PHASE1)
					nave.dir = 1;
				else if(current_state == GAME_PHASE2)
					nave2.dir = 1;
				break;
			case ALLEGRO_KEY_SPACE:
				if (current_state == GAME_PHASE1) {
					if (!temTiro) {
						initTiro(&tiro, nave.x);
						temTiro = 1;
					}
				} else if (current_state == GAME_PHASE2) {
					// Procurar um slot livre para lançar novo tiro
					for (i = 0; i < 3; i++) {
						if (!temTiro_fase2[i]) {
							initTiro(&tiros_fase2[i], nave2.x);
							temTiro_fase2[i] = 1;
							break; // Só lança um tiro por vez
						}
					}
				}
				break;
			default:
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) { 
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_A:
				if (current_state == GAME_PHASE1) {
					nave.esq = 0;
				} else if (current_state == GAME_PHASE2) {
					nave2.esq = 0;
				}
				break;
			case ALLEGRO_KEY_D:
				if (current_state == GAME_PHASE1) {
					nave.dir = 0;
				} else if (current_state == GAME_PHASE2) {
					nave2.dir = 0;
				}
				break;
			
			default:
				break;
			}
		}
	}
	
	if(ganhou){
		float record;
	
		al_clear_to_color(al_map_rgb(139, 179, 217));
		char my_score[20];
		char time[20];
		sprintf(my_score, "Score: %d", pontos);
		al_draw_text(size_f, al_map_rgb(200, 0, 30), SCREEN_W/3, 200, 0, my_score);
		if(newRecord(al_get_timer_count(timer) / FPS, &record)) {
			al_draw_text(size_f, al_map_rgb(200, 20, 30), SCREEN_W/3, 300, 0, "NEW TIME RECORD!");
			sprintf(time, "%.2f", al_get_timer_count(timer) / FPS);
			al_draw_text(size_f, al_map_rgb(200, 0, 30), SCREEN_W/3, 400, 0, time);
		}
		else {
			sprintf(time, "Record: %.2f", record);
			al_draw_text(size_f, al_map_rgb(0, 200, 30), SCREEN_W/3,300, 0, time);
		}
		al_flip_display();	
		al_rest(7);
	}

	if(perdeu){
		perdeu_sprite = al_load_bitmap("assets/perdeu.png");
		al_draw_bitmap(perdeu_sprite, 0, 0, 0);
		al_flip_display();	
		al_rest(7);
	}
	
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_bitmap(alien_sprite);
	al_destroy_bitmap(space_sprite);  
	al_destroy_bitmap(space2_sprite);
	al_destroy_bitmap(earth_sprite);
	al_destroy_bitmap(nave_sprite);
	al_destroy_bitmap(perdeu_sprite);
	al_destroy_bitmap(fase1_sprite);
	al_destroy_bitmap(fase2_sprite);
	al_destroy_font(size_f);
 
	return 0;
}