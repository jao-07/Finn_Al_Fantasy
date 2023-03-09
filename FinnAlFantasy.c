#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NAVEGACAO 0
#define BATALHA 1

#define TAM_HEROI 20
#define TAM_OBJ 100
#define PASSO 15
#define MAX_DIST 30

#define CIMA 0
#define BAIXO 1
#define ESQUERDA 2
#define DIREITA 3

#define NUM_MONSTROS 18
#define TAM_MONSTROS 20

#define FUGIR 0
#define ATACAR 1
#define ESPECIAL 2

const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

int DIRECAO;
int AUXCORTE;

int X_OPT;
int Y_OPT;

int Y_BOLA;
int Y_B;

int ATQ_P;
int ATQ_M;
int ESP_P;
int FUG_P;
int contAtq;

int ER_DX;
int EY_DX;
int BDF_DX;

int PONTUACAO;
char pont[20];

ALLEGRO_FONT *FONTE;
ALLEGRO_FONT *FONTE_FINAL;
int tam_fonte;

typedef struct Heroi {
	
	int x, y;
	
	//BATALHA//
	int hp;
	int acao;
	int executar;
	int x_old, y_old;
	
	
} Heroi;

typedef struct Monstro {
	
	int x, y;
	int numero;
	int hp;
	float tamanho;
	int morto;
	
} Monstro;

//Aleatório entre 0 e n

int random(int n) {
	
	return rand()%(n+1);
}

void initHeroi(Heroi *h){
	
	h->x =  0;
	h->y =  SCREEN_H - 45;
	
	h->hp = 200;
	h->acao = FUGIR;
	h->executar = 0;
}

void initMonstros(Monstro m[NUM_MONSTROS]) {
	int i=0;
	for(i=0; i<NUM_MONSTROS; i++) {
		m[i].x = random(SCREEN_W - TAM_MONSTROS);
		m[i].y = random(SCREEN_H - TAM_MONSTROS);
		if(m[i].x >= SCREEN_W - (TAM_OBJ + 165) && m[i].y + TAM_MONSTROS <= (TAM_OBJ + 90)){
			i--;
			continue;
		}
		if(m[i].x <= TAM_HEROI + 50 && m[i].y >= SCREEN_H - TAM_HEROI - 50){
			i--;
			continue;
		}
		printf("Coord monstro %d : (%d, %d)\n", i+1, m[i].x, m[i].y);
		m[i].numero = i+1;
		
		m[i].hp = 40 + random(160);
		m[i].tamanho = (m[i].hp * 7) / 10;
		m[i].morto = 0;
		printf("HP monstro %d : %d\n", i+1, m[i].hp);
	}
}

void initGlobais(){
	
	X_OPT = 77 * SCREEN_W / 100;
	Y_OPT = 6 * SCREEN_H / 10;
	
	Y_BOLA = SCREEN_H / 9;
	Y_B = 0;
	
	ATQ_P = 0;
	ATQ_M = 0; 
	ESP_P = 0;
	FUG_P = 0;
	contAtq = 0;
	
	DIRECAO = BAIXO;
	AUXCORTE = 0;
	
	ER_DX = 0;
	EY_DX = 0;
	BDF_DX = 0;
	
	PONTUACAO = 0;
	
	tam_fonte = SCREEN_W / 30;
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("arial.ttf", tam_fonte, 1);   
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE_FINAL = al_load_font("arial.ttf", (tam_fonte * 3) / 2, 1);   
	if(FONTE_FINAL == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
}

float dist(int x1, int y1, int x2, int y2){
	
	return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

//////////////////// - NAVEGACAO - ////////////////////

void desenhaHeroiNaveg(Heroi h, ALLEGRO_BITMAP *f){
	if(DIRECAO == BAIXO){
		al_draw_bitmap_region(f, 6 + AUXCORTE * 60, 0, 30, 45, h.x, h.y, 0);
	}
	else if(DIRECAO == DIREITA){
		al_draw_bitmap_region(f, 5 + AUXCORTE * 61, 45, 30, 45, h.x, h.y, 0);
	}
	else if(DIRECAO == ESQUERDA){
		al_draw_bitmap_region(f, 6 + AUXCORTE * 61, 90, 30, 45, h.x, h.y, 0);
	}
	else if(DIRECAO == CIMA){
		al_draw_bitmap_region(f, 6 + AUXCORTE * 57, 138, 30, 45, h.x, h.y, 0);
	}
	
}

void desenhaCenarioNaveg(ALLEGRO_BITMAP *c, ALLEGRO_BITMAP *o) {
	
	al_draw_bitmap(c, 0, 0, 0);
	al_draw_bitmap(o, SCREEN_W - 245, 0, 0);
	
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), 10, 10, 0, pont);
	
}

void processaTeclaNaveg(Heroi *h, int tecla) {
	
	h->x_old = h->x;
	h->y_old = h->y;
	
	switch(tecla) {
		
		case ALLEGRO_KEY_UP:
			if(h->y - PASSO >= 0){
				h->y -= PASSO;
				DIRECAO = CIMA;
				AUXCORTE++;
			}
			break;

		case ALLEGRO_KEY_DOWN:
			if(h->y + PASSO + 45 <= SCREEN_H){		
				h->y += PASSO;
				DIRECAO = BAIXO;
				AUXCORTE++;
			}
			break;

		case ALLEGRO_KEY_LEFT: 
			if(h->x - PASSO >= 0){
				h->x -= PASSO;
				DIRECAO = ESQUERDA;
				AUXCORTE++;
			}
			break;

		case ALLEGRO_KEY_RIGHT:
			if(h->x + PASSO + 30 <= SCREEN_W){
				h->x += PASSO;
				DIRECAO = DIREITA;
				AUXCORTE++;
			}
			break;
	}
	
	if(AUXCORTE > 1)
		AUXCORTE = 0;
}

int detectouMonstro(Heroi *h, Monstro m[NUM_MONSTROS]){
	int i=0;
	for(i=0; i<NUM_MONSTROS; i++){
		if(dist(h->x , h->y + 22, m[i].x, m[i].y) <= MAX_DIST){
			if(m[i].morto == 1){
				return 0;
			}
			else 
				return m[i].numero;
		}
	}
	return 0;
}

int chegouObjetivo(Heroi h){
	
	if(h.x + 15 >= SCREEN_W - 245 && h.y + 22 + TAM_HEROI <= 174)
		return 1;
	return 0;
}

//////////////////// - BATALHA - ////////////////////

void imagem_monstro(int n, Monstro m[NUM_MONSTROS], ALLEGRO_BITMAP *v1, ALLEGRO_BITMAP *v2, ALLEGRO_BITMAP *v3, ALLEGRO_BITMAP *v4, ALLEGRO_BITMAP *v5, ALLEGRO_BITMAP *v6, ALLEGRO_BITMAP *v7, ALLEGRO_BITMAP *v8, ALLEGRO_BITMAP *v9, ALLEGRO_BITMAP *v10, ALLEGRO_BITMAP *v11, ALLEGRO_BITMAP *v12, ALLEGRO_BITMAP *v13, ALLEGRO_BITMAP *v14, ALLEGRO_BITMAP *v15, ALLEGRO_BITMAP *v16, ALLEGRO_BITMAP *v17, ALLEGRO_BITMAP *v18){
	
	switch(m[n-1].numero){
		//Lich
		case 1:
			al_draw_bitmap(v1, 330, 170, 0);
			break;
		//Arvore
		case 2:
			al_draw_bitmap(v2, 430, 200, 0);
			break;
		//Darren
		case 3:
			al_draw_bitmap(v3, 430, 170, 0);
			break;
		//GOLB
		case 4:
			al_draw_bitmap(v17, 480, 300, 0);
			break;
		//Hunson Abdeer
		case 5:
			al_draw_bitmap(v5, 450, 210, 0);
			break;
		//Qué ota
		case 6:
			al_draw_bitmap(v6, 430, 200, 0);
			break;
		//Alce
		case 7:
			al_draw_bitmap(v7, 520, 230, 0);
			break;
		//Lobo 
		case 8:
			al_draw_bitmap(v8, 430, 240, 0);
			break;
		//Lorde
		case 9:
			al_draw_bitmap(v9, 430, 200, 0);
			break;
		//Mago
		case 10:
			al_draw_bitmap(v10, 480, 240, 0);
			break;
		//MeMow
		case 11:
			al_draw_bitmap(v11, 500, 350, 0);
			break;
		//Morte
		case 12:
			al_draw_bitmap(v12, 550, 200, 0);
			break;
		//Rainha
		case 13:
			al_draw_bitmap(v13, 470, 200, 0);
			break;
		//Rei fogo
		case 14:
			al_draw_bitmap(v14, 480, 200, 0);
			break;
		//Rei gelo
		case 15:
			al_draw_bitmap(v15, 430, 170, 0);
			break;
		//Rei minhoca
		case 16:
			al_draw_bitmap(v16, 400, 150, 0);
			break;
		//Ricardio
		case 17:
			al_draw_bitmap(v4, 430, 170, 0);
			break;
		//Tiffany
		case 18:
			al_draw_bitmap(v18, 430, 200, 0);
			break;
	}
}

void drawMonstro(int n, Monstro m[NUM_MONSTROS]){
	
	al_draw_filled_rounded_rectangle(700, 20, 950, 100, 5, 5, al_map_rgb(255, 219, 88));
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), 705, 25, 0, "HP Monstro");
	al_draw_filled_rounded_rectangle(705, 65, 705 + m[n - 1].tamanho * 10 / 7 + 8, 90, 5, 5, al_map_rgb(255, 255, 255));
	al_draw_filled_rounded_rectangle(709, 69, 709 + m[n - 1].hp , 86, 5, 5, al_map_rgb(255, 0, 0));
	
}

void drawPlayer(Heroi *h){
	al_draw_filled_rounded_rectangle(20, 20, 270, 100, 5, 5, al_map_rgb(255, 219, 88));
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), 25, 25, 0, "HP Player");
	al_draw_filled_rounded_rectangle(25, 65, 225, 90, 5, 5, al_map_rgb(255, 255, 255));
	al_draw_filled_rounded_rectangle(29, 69, 29 + (h->hp - 8), 86, 5, 5, al_map_rgb(255, 0, 0));
}

void desenhaCenarioBatalha(Heroi *h, Monstro m[NUM_MONSTROS], ALLEGRO_BITMAP *fundo1, ALLEGRO_BITMAP *fundo2, ALLEGRO_BITMAP *fundo3, ALLEGRO_BITMAP *v1, ALLEGRO_BITMAP *v2, ALLEGRO_BITMAP *v3, ALLEGRO_BITMAP *v4, ALLEGRO_BITMAP *v5, ALLEGRO_BITMAP *v6, ALLEGRO_BITMAP *v7, ALLEGRO_BITMAP *v8, ALLEGRO_BITMAP *v9, ALLEGRO_BITMAP *v10, ALLEGRO_BITMAP *v11, ALLEGRO_BITMAP *v12, ALLEGRO_BITMAP *v13, ALLEGRO_BITMAP *v14, ALLEGRO_BITMAP *v15, ALLEGRO_BITMAP *v16, ALLEGRO_BITMAP *v17, ALLEGRO_BITMAP *v18){
	if(m[detectouMonstro(&(*h), m) - 1].numero <= 6)
		al_draw_bitmap(fundo1, 0, 0, 0);
	else if(m[detectouMonstro(&(*h), m) - 1].numero > 6 && m[detectouMonstro(&(*h), m) - 1].numero <= 12)
		al_draw_bitmap(fundo2, 0, 0, 0);
	else if(m[detectouMonstro(&(*h), m) - 1].numero >12)
		al_draw_bitmap(fundo3, 0, 0, 0);

	drawPlayer(h);
	imagem_monstro(detectouMonstro(&(*h), m), m, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18);
	drawMonstro(detectouMonstro(&(*h), m), m);
	
	al_draw_filled_rounded_rectangle(X_OPT, Y_OPT, SCREEN_W - 30, SCREEN_H - 30, 15, 15, al_map_rgb(255, 0, 0));
	al_draw_filled_rounded_rectangle(X_OPT + 5, Y_OPT + 5, SCREEN_W - 35, SCREEN_H - 35, 15, 15, al_map_rgb(255, 255, 255));
	
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), X_OPT + 45, Y_OPT + 13, 0, "Fugir");
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), X_OPT + 45, Y_OPT + 73, 0, "Ataque");
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), X_OPT + 45, Y_OPT + 133, 0, "Especial");
	
	al_draw_filled_circle(X_OPT + 25, Y_OPT + 32 + (Y_BOLA * Y_B) , 7, al_map_rgb(0, 255, 0));
}

void processaTeclaBatalha(Heroi *h, int tecla) {
	
		switch(tecla) {
		
		case ALLEGRO_KEY_UP:
			if(Y_B == FUGIR)
				Y_B = ESPECIAL;
			else
				Y_B--;
			break;
		
		case ALLEGRO_KEY_DOWN: 
			if(Y_B == ESPECIAL)
				Y_B = FUGIR;
			else
				Y_B++;
			break;	
		
		case ALLEGRO_KEY_ENTER: 
			h->executar = 1;
			break;
	}
	
}

void atacaHeroi(Heroi *h, Monstro m[NUM_MONSTROS], int n){
	int dano = 20 + random(10);
	
	if(m[n-1].hp - dano >0)
		m[n-1].hp -= dano;
	else
		m[n-1].hp = 0;
	
	if(m[n-1].hp == 0){
		m[n-1].morto = 1;
		
	}
	
	contAtq++;
}

void especialHeroi(Heroi *h, Monstro m[NUM_MONSTROS], int n){
	int dano = 60 + random(30);
	
	if(m[n-1].hp - dano >0)
		m[n-1].hp -= dano;
	else
		m[n-1].hp = 0;
	
	if(m[n-1].hp == 0){
		m[n-1].morto = 1;
		
	}
	contAtq = 0;
}

void atacaMonstro(Heroi *h, Monstro m[NUM_MONSTROS], int n){
	
	int dano = (m[n-1].tamanho / 10) +  random(10);
	
	if(h->hp - dano > 0)
		h->hp -= dano;
	else
		h->hp = 0;
}

int processaAcaoHeroi(Heroi *h, Monstro m[NUM_MONSTROS], int n) {
	int p;
	if(h->executar) {
		
		h->executar = 0;
		
		if(Y_B == FUGIR)
			h->acao = FUGIR;
		else if(Y_B == ATACAR)
			h->acao = ATACAR;
		else if(Y_B == ESPECIAL)
			h->acao = ESPECIAL;
		
		if(h->acao == FUGIR){
			p = random(99);
			if(p < 30){
				h->x = h->x_old;
				h->y = h->y_old;
			
				return NAVEGACAO;
			}
			else
				FUG_P = 1;
		}
		else if(h->acao == ATACAR){
			ATQ_P = 1;
		}
		else if(h->acao == ESPECIAL){
			if(contAtq >= 3)
				ESP_P = 1;
			
		}
	}
	return BATALHA; 
}

///////////////////////////////////////////////////////////
 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	
	ALLEGRO_BITMAP *finn_naveg = NULL;
	ALLEGRO_BITMAP *fundo_naveg = NULL;
	ALLEGRO_BITMAP *casa_arvore = NULL;
	
	ALLEGRO_BITMAP *finn_player = NULL;
	ALLEGRO_BITMAP *finn_player2 = NULL;
	ALLEGRO_BITMAP *bola_de_fogo = NULL;
	ALLEGRO_BITMAP *espada_red = NULL;
	ALLEGRO_BITMAP *yellow_sword = NULL;
	ALLEGRO_BITMAP *fundo_batalha = NULL;
	ALLEGRO_BITMAP *fundo_batalha2 = NULL;
	ALLEGRO_BITMAP *fundo_batalha3 = NULL;
	
	ALLEGRO_BITMAP *alce = NULL;
	ALLEGRO_BITMAP *arvore_tenebrosa = NULL;
	ALLEGRO_BITMAP *darren = NULL;
	ALLEGRO_BITMAP *golb = NULL;
	ALLEGRO_BITMAP *hunson_abdeer = NULL;
	ALLEGRO_BITMAP *kee_oth = NULL;
	ALLEGRO_BITMAP *lich = NULL;
	ALLEGRO_BITMAP *lobo_do_abraco = NULL;
	ALLEGRO_BITMAP *lorde_das_portas = NULL;
	ALLEGRO_BITMAP *mago_da_floresta = NULL;
	ALLEGRO_BITMAP *me_mow = NULL;
	ALLEGRO_BITMAP *morte = NULL;
	ALLEGRO_BITMAP *rainha_gelada = NULL;
	ALLEGRO_BITMAP *rei_de_fogo = NULL;
	ALLEGRO_BITMAP *rei_gelado = NULL;
	ALLEGRO_BITMAP *rei_minhoca = NULL;
	ALLEGRO_BITMAP *ricardio = NULL;
	ALLEGRO_BITMAP *tiffany = NULL;
	
	ALLEGRO_BITMAP *tela_final = NULL;
	ALLEGRO_BITMAP *tela_final_lose = NULL;
	
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
	
	//inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	


	//inicia o temporizador
	al_start_timer(timer);
	
	
	//BITMAPS//
	
	
	//carregar a figura .png na variavel espada_red
    espada_red = al_load_bitmap("bitmaps//espada_red.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!espada_red) {
      fprintf(stderr, "failed to create espada_red bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel espada_red
    yellow_sword = al_load_bitmap("bitmaps//yellow_sword.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!yellow_sword) {
      fprintf(stderr, "failed to create yellow_sword bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel bola_de_fogo
    bola_de_fogo = al_load_bitmap("bitmaps//bola_de_fogo.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!bola_de_fogo) {
      fprintf(stderr, "failed to create bola_de_fogo bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel finn_player
    finn_player = al_load_bitmap("bitmaps//finn_player.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!finn_player) {
      fprintf(stderr, "failed to create finn_player bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel finn_player2
    finn_player2 = al_load_bitmap("bitmaps//finn_pulo.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!finn_player2) {
      fprintf(stderr, "failed to create finn_player2 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel finn_naveg
    finn_naveg = al_load_bitmap("bitmaps//finn_naveg.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!finn_naveg) {
      fprintf(stderr, "failed to create finn_naveg bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel fundo_batalha
    fundo_batalha = al_load_bitmap("bitmaps//floresta_rosa.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!fundo_batalha) {
      fprintf(stderr, "failed to create fundo_batalha bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel fundo_batalha2
    fundo_batalha2 = al_load_bitmap("bitmaps//floresta.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!fundo_batalha2) {
      fprintf(stderr, "failed to create fundo_batalha2 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel fundo_batalha3
    fundo_batalha3 = al_load_bitmap("bitmaps//noite.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!fundo_batalha3) {
      fprintf(stderr, "failed to create fundo_batalha3 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel fundo_batalha
    fundo_naveg = al_load_bitmap("bitmaps//fundo_naveg.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!fundo_naveg) {
      fprintf(stderr, "failed to create fundo_naveg bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel tela_final
    tela_final = al_load_bitmap("bitmaps//tela_final.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!tela_final) {
      fprintf(stderr, "failed to create tela_final bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel tela_final
    tela_final_lose = al_load_bitmap("bitmaps//tela_final_lose.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!tela_final_lose) {
      fprintf(stderr, "failed to create tela_final_lose bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel casa_arvore
    casa_arvore = al_load_bitmap("bitmaps//casa_arvore.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!casa_arvore) {
      fprintf(stderr, "failed to create casa_arvore bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//VILÕES//
	
	//carregar a figura .png na variavel alce
    alce = al_load_bitmap("bitmaps//alce.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!alce) {
      fprintf(stderr, "failed to create alce bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel arvore_tenebrosa
    arvore_tenebrosa = al_load_bitmap("bitmaps//arvore_tenebrosa.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!arvore_tenebrosa) {
      fprintf(stderr, "failed to create arvore_tenebrosa bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel darren
    darren = al_load_bitmap("bitmaps//darren.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!darren) {
      fprintf(stderr, "failed to create darren bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel golb
    golb = al_load_bitmap("bitmaps//golb.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!golb) {
      fprintf(stderr, "failed to create golb bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel hunson_abdeer
    hunson_abdeer = al_load_bitmap("bitmaps//hunson_abdeer.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!hunson_abdeer) {
      fprintf(stderr, "failed to create hunson_abdeer bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel kee_oth
    kee_oth = al_load_bitmap("bitmaps//kee_oth.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!kee_oth) {
      fprintf(stderr, "failed to create kee_oth bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel lich
    lich = al_load_bitmap("bitmaps//lich.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!lich) {
      fprintf(stderr, "failed to create lich bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel lobo_do_abraco
    lobo_do_abraco = al_load_bitmap("bitmaps//lobo_do_abraco.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!lobo_do_abraco) {
      fprintf(stderr, "failed to create lobo_do_abraco bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel lorde_das_portas
    lorde_das_portas = al_load_bitmap("bitmaps//lorde_das_portas.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!lorde_das_portas) {
      fprintf(stderr, "failed to create lorde_das_portas bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel mago_da_floresta
    mago_da_floresta = al_load_bitmap("bitmaps//mago_da_floresta.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!mago_da_floresta) {
      fprintf(stderr, "failed to create mago_da_floresta bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel me_mow
    me_mow = al_load_bitmap("bitmaps//me_mow.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!me_mow) {
      fprintf(stderr, "failed to create me_mow bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel morte
    morte = al_load_bitmap("bitmaps//morte.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!morte) {
      fprintf(stderr, "failed to create morte bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel rainha_gelada
    rainha_gelada = al_load_bitmap("bitmaps//rainha_gelada.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!rainha_gelada) {
      fprintf(stderr, "failed to create rainha_gelada bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel rei_de_fogo
    rei_de_fogo = al_load_bitmap("bitmaps//rei_de_fogo.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!rei_de_fogo) {
      fprintf(stderr, "failed to create rei_de_fogo bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel rei_gelado
    rei_gelado = al_load_bitmap("bitmaps//rei_gelado.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!rei_gelado) {
      fprintf(stderr, "failed to create rei_gelado bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel rei_minhoca
    rei_minhoca = al_load_bitmap("bitmaps//rei_minhoca.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!rei_minhoca) {
      fprintf(stderr, "failed to create rei_minhoca bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel ricardio
    ricardio = al_load_bitmap("bitmaps//ricardio.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!ricardio) {
      fprintf(stderr, "failed to create ricardio bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	//carregar a figura .png na variavel tiffany
    tiffany = al_load_bitmap("bitmaps//tiffany.png");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!tiffany) {
      fprintf(stderr, "failed to create tiffany bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	
	int playing = 1;
	int modo_jogo = NAVEGACAO;
	FILE *arq;
	
	Heroi heroi;
	Monstro monstros[NUM_MONSTROS];
	
	srand(100);
	initGlobais();
	initHeroi(&heroi);
	initMonstros(monstros);
	
	
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			if(modo_jogo == NAVEGACAO) {
				
				desenhaCenarioNaveg(fundo_naveg,casa_arvore);
				desenhaHeroiNaveg(heroi, finn_naveg);
				sprintf(pont, "Pontuação: %d", PONTUACAO);
				
				if(detectouMonstro(&heroi, monstros)) {
					modo_jogo = BATALHA;
				}
				
				if(chegouObjetivo(heroi)){
					playing = 0;
					PONTUACAO++;
				}
			}
			else {
				modo_jogo = processaAcaoHeroi(&heroi, monstros, detectouMonstro(&heroi, monstros));
				int n = detectouMonstro(&heroi, monstros);
				
				if(ATQ_P == 1){
					if(ATQ_M == 0){
						ER_DX += 10;
						if(ER_DX > 400){
							atacaHeroi(&heroi, monstros, n);
							if(monstros[n-1].hp == 0){
								ATQ_M = 0;
								ATQ_P = 0;
								ER_DX = 0;
								BDF_DX = 0;
								PONTUACAO += monstros[n-1].tamanho;
							}
							else
								ATQ_M = 1;
						}	
					}
					else{
						BDF_DX += 10;
						if(BDF_DX > 400){
							atacaMonstro(&heroi, monstros, n);
							ATQ_P = 0;
							ATQ_M = 0;
							ER_DX = 0;
							BDF_DX = 0;
						}
					}
				} 
				else if(ESP_P == 1){
					if(ATQ_M == 0){
						EY_DX += 10;
						if(EY_DX > 400){
							especialHeroi(&heroi, monstros, n);
							if(monstros[n-1].hp == 0){
								ATQ_M = 0;
								EY_DX = 0;
								BDF_DX = 0;
								ESP_P = 0;
								PONTUACAO += monstros[n - 1].tamanho;
							}
							else
								ATQ_M = 1;
						}
					}
					else{
						BDF_DX += 10;
						if(BDF_DX > 400){
							atacaMonstro(&heroi, monstros, n);
							ESP_P = 0;
							ATQ_M = 0;
							EY_DX = 0;
							BDF_DX = 0;
						}
					}
				}
				else if(FUG_P == 1){
					
					ATQ_M = 1;
					BDF_DX += 10;
						if(BDF_DX > 400){
							atacaMonstro(&heroi, monstros, n);
							ATQ_M = 0;
							BDF_DX = 0;
							FUG_P = 0;
						}
				}
				desenhaCenarioBatalha(&heroi, monstros, fundo_batalha, fundo_batalha2, fundo_batalha3, lich, arvore_tenebrosa, darren, golb, hunson_abdeer, kee_oth, alce, lobo_do_abraco, lorde_das_portas, mago_da_floresta, me_mow, morte, rainha_gelada, rei_de_fogo, rei_gelado, rei_minhoca, ricardio, tiffany);
				
				if(ATQ_P == 0 && ESP_P == 0){
					al_draw_bitmap(finn_player, 30, 200, 0);
				}
				else{
					al_draw_bitmap(finn_player2, 30, 200, 0);
					
					if(ATQ_P == 1){
						if(ER_DX < 400){ 
							al_draw_bitmap(yellow_sword, 130 + ER_DX, 350, 0);
						}
					}
					else if(ESP_P == 1){
						if(EY_DX < 400)
							al_draw_bitmap(espada_red, 130 + EY_DX, 350, 0);
					}
				}
				if(ATQ_M == 1){
					if(BDF_DX < 400)
						al_draw_bitmap(bola_de_fogo, 500 - BDF_DX, 350, 0);
				}
				if(FUG_P == 1){
					if(BDF_DX < 400)
						al_draw_text(FONTE, al_map_rgb(0, 0, 0), 300, 120, 0, "Falha na fuga, Marreco!");
				}
				if(monstros[n-1].hp == 0){
					modo_jogo = NAVEGACAO;
					ATQ_P = 0;
				}
				
				if(heroi.hp == 0)
					playing = 0;
			}

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}
		
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			
			if(modo_jogo == NAVEGACAO) {
				
				processaTeclaNaveg(&heroi, ev.keyboard.keycode);
				
			}
			else {
				
				processaTeclaBatalha(&heroi, ev.keyboard.keycode);
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		
		}
	} //fim do while
	
	//Tela de final de jogo
	
	if(!chegouObjetivo(heroi)){
		al_draw_bitmap(tela_final_lose, 0, 0, 0);
	}
	else{
		al_draw_bitmap(tela_final, 0, 0, 0);
		
		int rec = 0;
		char r[20];
		
		sprintf(r, "Pontuação Final: %d", PONTUACAO);
		al_draw_text(FONTE_FINAL, al_map_rgb(0, 0, 0), 3 * SCREEN_W / 10, 19 * SCREEN_H / 100, 0, r);
		
		arq = fopen("recorde.txt", "r");
		fscanf(arq, "%d", &rec);
		
		if(PONTUACAO > rec){
			arq = fopen("recorde.txt", "w");
			fprintf(arq, "%d", PONTUACAO);
			al_draw_text(FONTE_FINAL, al_map_rgb(0, 0, 0), 3 * SCREEN_W / 10, 35 * SCREEN_H / 100, 0, "Novo Recorde!!!");
		}
		fclose(arq);
	}
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(5);
	
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
	al_destroy_bitmap(finn_naveg);
	al_destroy_bitmap(fundo_naveg);
	al_destroy_bitmap(casa_arvore);
	al_destroy_bitmap(finn_player);
	al_destroy_bitmap(finn_player2);
	al_destroy_bitmap(bola_de_fogo);
	al_destroy_bitmap(espada_red);
	al_destroy_bitmap(yellow_sword);
	al_destroy_bitmap(fundo_batalha);
	al_destroy_bitmap(fundo_batalha2);
	al_destroy_bitmap(fundo_batalha3);
	al_destroy_bitmap(alce);
	al_destroy_bitmap(arvore_tenebrosa);
	al_destroy_bitmap(darren);
	al_destroy_bitmap(golb);
	al_destroy_bitmap(hunson_abdeer);
	al_destroy_bitmap(kee_oth);
	al_destroy_bitmap(lich);
	al_destroy_bitmap(lobo_do_abraco);
	al_destroy_bitmap(lorde_das_portas);
	al_destroy_bitmap(mago_da_floresta);
	al_destroy_bitmap(me_mow);
	al_destroy_bitmap(morte);
	al_destroy_bitmap(rainha_gelada);
	al_destroy_bitmap(rei_de_fogo);
	al_destroy_bitmap(rei_gelado);
	al_destroy_bitmap(rei_minhoca);
	al_destroy_bitmap(ricardio);
	al_destroy_bitmap(tiffany);
	al_destroy_bitmap(tela_final);
	al_destroy_bitmap(tela_final_lose);
 
	return 0;
}
