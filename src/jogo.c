#include "../include/jogo.h"
#include "../include/tipos.h"
#include "../include/config.h"
#include "../include/pedidos.h"
#include "../include/historico.h"
#include "../include/relatorio.h"
#include "../include/fila.h"
#include "../include/pilha.h"
#include "../include/lista.h"
#include "../include/heap.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TAM_ENTRADA 256

// simbolos de cada categoria, a posicao no vetor e o indice no estoque
static const char PERENES[]    = "sqct";
static const char PERECIVEIS[] = "nob";
static const char INFINITOS[]  = "zkmpa";
static const int  VALIDADE[3]  = { CFG_VALIDADE_CARNE, CFG_VALIDADE_OVO, CFG_VALIDADE_BACON };

// contador pros perenes e um heap pra cada perecivel
typedef struct {
    int  perenes[4];
    Heap pereciveis[3];
} Estoque;

static const char* nome_ingrediente(char s){
    switch (s){
        case ING_PAO_CIMA:       
            return "pão de cima";
        case ING_KETCHUP:        
            return "ketchup";
        case ING_MOSTARDA:       
            return "mostarda";
        case ING_PICLES:         
            return "picles";
        case ING_ALFACE:         
            return "alface";
        case ING_QUEIJO_SUICO:   
            return "queijo suíço";
        case ING_QUEIJO_CHEDDAR: 
            return "queijo cheddar";
        case ING_CEBOLA:         
            return "cebola";
        case ING_TOMATE:         
            return "tomate";
        case ING_CARNE:          
            return "carne";
        case ING_OVO_FRITO:      
            return "ovo frito";
        case ING_BACON:          
            return "bacon";
    }
    return "?";
}

// indice do simbolo no vetor, -1 se nao tem
static int posicao(const char *vet, char s){
    for (int i = 0; vet[i] != '\0'; i++){
        if (vet[i] == s){
            return i;
        }
    }
    return -1;
}

static int categoria(char s){
    if (posicao(INFINITOS, s) >= 0){
        return CAT_INFINITO;
    }
    if (posicao(PERENES, s) >= 0){
        return CAT_PERENE;
    }
    if (posicao(PERECIVEIS, s) >= 0){
        return CAT_PERECIVEL;
    }
    return CAT_INVALIDO;
}

/* ==========================================================================
 * PARTE VISUAL (raylib)
 * tudo daqui ate a proxima secao so desenha. a cena guarda uma COPIA do que
 * aparece na tela, entao nada aqui mexe na fila, na pilha, na lista ou no heap
 * ========================================================================== */

#define LARGURA_TELA  1280
#define ALTURA_TELA   720

#define RX            365     // x onde comeca o restaurante (o mapa)
#define MAX_CAMADAS   64      // so pra copia do prato que vai pra tela
#define NUM_ASSENTOS  6       // cadeiras do salao
#define FILA_Y        362     // y do cliente da frente
#define FILA_PASSO    46      // distancia entre clientes na fila
#define FILA_VISIVEL  8       // quantos cabem desenhados na fila
#define LARGURA_PAO   144     // hamburguer grande: 48 px da arte * 3

// cores (as do mapa seguem a legenda do desenho)
#define COR_FUNDO       (Color){ 245, 238, 226, 255 }
#define COR_PAINEL      (Color){ 255, 251, 243, 255 }
#define COR_BORDA       (Color){ 222, 208, 188, 255 }
#define COR_TEXTO       (Color){  40,  36,  32, 255 }
#define COR_SUAVE       (Color){ 128, 116, 104, 255 }
#define COR_LARANJA     (Color){ 232, 128,  20, 255 }
#define COR_PISO_SALAO  (Color){ 250, 245, 234, 255 }
#define COR_PISO_A      (Color){ 232, 232, 238, 255 }
#define COR_PISO_B      (Color){ 220, 220, 228, 255 }
#define COR_PAREDE      (Color){  26,  26,  26, 255 }
#define COR_MESA        (Color){  52, 204,  96, 255 }
#define COR_MESA_ESC    (Color){  30, 150,  66, 255 }
#define COR_CLIENTE     (Color){  66, 133, 244, 255 }
#define COR_CLIENTE_ESC (Color){  34,  88, 186, 255 }
#define COR_BRAVO       (Color){ 228,  60,  50, 255 }
#define COR_JOGADOR     (Color){ 246, 146,  30, 255 }
#define COR_JOGADOR_ESC (Color){ 180,  92,   8, 255 }
#define COR_MONTAGEM    (Color){ 250, 204,  21, 255 }
#define COR_MONTAGEM_ESC (Color){ 204, 156,  8, 255 }
#define COR_ENTREGA     (Color){ 182,  76, 204, 255 }
#define COR_ENTREGA_ESC (Color){ 128,  44, 150, 255 }
#define COR_CHAPA       (Color){ 228,  62,  50, 255 }
#define COR_CHAPA_FUNDO (Color){  70,  60,  60, 255 }
#define COR_LIXEIRA     (Color){ 110, 110, 110, 255 }
#define COR_LIXEIRA_ESC (Color){  70,  70,  70, 255 }
#define COR_PRATO       (Color){ 236, 233, 226, 255 }
#define COR_PRATO_ESC   (Color){ 200, 196, 188, 255 }
#define COR_OK          (Color){  40, 150,  70, 255 }
#define COR_ERRO        (Color){ 200,  56,  48, 255 }
#define COR_INFO        (Color){  60,  54,  48, 255 }

typedef enum { POS_INICIO, POS_CHAPA, POS_MONTAGEM, POS_ENTREGA, POS_LIXEIRA } Posicao;
typedef enum { RES_JOGANDO, RES_VITORIA, RES_DERROTA, RES_SAIU } Resultado;

// onde o jogador "teleporta" em cada acao
static const Vector2 POSICOES[5] = {
    { RX + 300, 612 },   // comeco, meio da cozinha
    { RX + 131, 540 },   // na frente da chapa
    { RX + 469, 540 },   // na frente da mesa de montagem
    { RX + 300, 540 },   // na frente da mesa de entrega
    { RX + 116, 676 },   // do lado da lixeira
};

// moveis do mapa
static const Rectangle R_CHAPA    = { RX + 27,  407, 208, 88 };
static const Rectangle R_ENTREGA  = { RX + 235, 407, 130, 88 };
static const Rectangle R_MONTAGEM = { RX + 365, 407, 208, 88 };
static const Vector2   P_LIXEIRA  = { RX + 58,  682 };

static const Rectangle MESAS[4] = {
    { RX + 60,  100, 80, 70 },
    { RX + 460,  90, 80, 70 },
    { RX + 110, 250, 80, 70 },
    { RX + 410, 240, 80, 70 },
};

// cadeiras na ordem em que os clientes sentam
static const Rectangle ASSENTOS[NUM_ASSENTOS] = {
    { RX + 80,  274, 22, 22 },
    { RX + 198, 274, 22, 22 },
    { RX + 380, 264, 22, 22 },
    { RX + 498, 264, 22, 22 },
    { RX + 30,  124, 22, 22 },
    { RX + 548, 114, 22, 22 },
};

// copia de tudo que aparece na tela
typedef struct {
    const Fila *fila;
    int  numero;
    int  turno;
    int  paciencia;
    int  codigo;
    int  atendidos;
    int  na_fila;
    char desejado[TAM_MAX_RECEITA + 1];
    char atual[MAX_CAMADAS + 1];          // prato de baixo pra cima, sem o pao de baixo
    int  perenes[4];
    int  qtd_pereciveis[3];
    int  validades[3][CFG_HEAP_CAP];      // turno em que vence, do mais perto pro mais longe
    Posicao   pos;
    Posicao   pos_anterior;
    double    t_mov;                      // quando teleportou (efeito)
    double    t_avanco;                   // quando a fila andou (efeito)
    char      aviso[160];
    Color     cor_aviso;
    double    t_aviso;
    Resultado resultado;
} Cena;

static Cena g_cena;

/* ---------- sprites do hamburguer ---------- */

// ordem das texturas, o '-' e o pao de baixo
static const char  CAMADAS[] = "zkmpasqctnob-";
#define IDX_PAO_BAIXO 12

// se existir assets/<nome>.png usa a imagem, senao gera a pixel art aqui mesmo
static const char *ARQUIVOS_CAMADA[13] = {
    "pao_cima", "ketchup", "mostarda", "picles", "alface", "queijo_suico",
    "cheddar", "cebola", "tomate", "carne", "ovo", "bacon", "pao_baixo"
};

static Texture2D g_tex[13];

static Color rgb(int r, int g, int b){
    return (Color){ (unsigned char) r, (unsigned char) g, (unsigned char) b, 255 };
}

// linha horizontal de x0 ate x1
static void lin(Image *im, int x0, int x1, int y, Color c){
    for (int x = x0; x <= x1; x++){
        ImageDrawPixel(im, x, y, c);
    }
}

// pixel art de cada camada, 48 px de largura (igual a folha de referencia)
static Image gerar_camada(char s){
    Image im;
    switch (s){
        case ING_PAO_CIMA: {
            static const int borda[13] = { 16, 12, 9, 7, 5, 4, 3, 2, 1, 1, 0, 0, 0 };
            static const int sementes[12][2] = {
                {14,3},{22,2},{30,3},{10,6},{19,5},{27,6},{36,5},{7,9},{15,9},{24,8},{32,9},{40,9}
            };
            Color base = rgb(224,155,70), claro = rgb(240,186,104), escuro = rgb(198,126,50);
            Color contorno = rgb(164,98,40), semente = rgb(252,240,206);
            im = GenImageColor(48, 17, BLANK);
            for (int y = 0; y < 13; y++){
                int e = borda[y], d = 47 - borda[y];
                lin(&im, e, d, y, base);
                if (y == 0){
                    lin(&im, e, d, y, escuro);
                }else{
                    lin(&im, e, borda[y - 1], y, escuro);
                    lin(&im, 47 - borda[y - 1], d, y, escuro);
                }
                if (y >= 1 && y <= 3){
                    lin(&im, e + 3, e + 8, y, claro);
                }
                if (y >= 4){
                    ImageDrawPixel(&im, d - 1, y, escuro);
                }
            }
            lin(&im, 0, 47, 13, claro);
            lin(&im, 0, 47, 14, base);
            lin(&im, 0, 47, 15, escuro);
            lin(&im, 1, 46, 16, contorno);
            for (int y = 13; y <= 15; y++){
                ImageDrawPixel(&im, 0, y, escuro);
                ImageDrawPixel(&im, 47, y, escuro);
            }
            for (int i = 0; i < 12; i++){
                lin(&im, sementes[i][0], sementes[i][0] + 1, sementes[i][1], semente);
            }
        } break;

        case ING_KETCHUP:        
        case ING_MOSTARDA: {
            Color claro  = (s == ING_KETCHUP) ? rgb(236,80,64)  : rgb(252,216,72);
            Color escuro = (s == ING_KETCHUP) ? rgb(194,36,32)  : rgb(226,174,24);
            int fase     = (s == ING_KETCHUP) ? 0 : 6;
            im = GenImageColor(48, 4, BLANK);
            for (int x = 2; x <= 45; x++){
                int f  = (x + fase) % 12;
                int dy = (f < 4) ? 0 : (f < 6) ? 1 : (f < 10) ? 2 : 1;
                ImageDrawPixel(&im, x, dy, claro);
                ImageDrawPixel(&im, x, dy + 1, escuro);
            }
        } break;

        case ING_PICLES: {
            Color base = rgb(98,158,58), escuro = rgb(62,110,34), claro = rgb(150,202,96);
            im = GenImageColor(48, 5, BLANK);
            for (int i = 0; i < 3; i++){
                int x0 = 4 + i * 14;
                lin(&im, x0 + 1, x0 + 10, 0, escuro);
                for (int y = 1; y <= 3; y++){
                    lin(&im, x0, x0 + 11, y, base);
                    ImageDrawPixel(&im, x0, y, escuro);
                    ImageDrawPixel(&im, x0 + 11, y, escuro);
                }
                lin(&im, x0 + 3, x0 + 8, 2, claro);
                lin(&im, x0 + 1, x0 + 10, 4, escuro);
            }
        } break;

        case ING_ALFACE: {
            static const int ponta[6] = { 5, 6, 7, 8, 7, 6 };
            Color base = rgb(118,204,78), claro = rgb(162,228,110), escuro = rgb(66,156,48);
            im = GenImageColor(48, 9, BLANK);
            for (int x = 0; x < 48; x++){
                int topo  = ((x / 3) % 2 == 0) ? 0 : 1;
                int fundo = ponta[x % 6];
                for (int y = topo; y <= fundo; y++){
                    Color c = base;
                    if (y >= fundo - 1){
                        c = escuro;
                    }else if (y == topo){
                        c = claro;
                    }
                    ImageDrawPixel(&im, x, y, c);
                }
            }
        } break;

        case ING_QUEIJO_SUICO:   
        case ING_QUEIJO_CHEDDAR: {
            static const int pingos_s[4][3] = { {3,8,4}, {15,20,5}, {27,32,4}, {38,43,5} };
            static const int pingos_q[4][3] = { {5,10,5}, {17,21,4}, {29,34,5}, {40,44,4} };
            int suico   = (s == ING_QUEIJO_SUICO);
            Color claro = suico ? rgb(252,238,170) : rgb(252,194,80);
            Color base  = suico ? rgb(246,224,128) : rgb(246,164,32);
            Color sombra = suico ? rgb(222,192,92) : rgb(214,130,18);
            const int (*pingos)[3] = suico ? pingos_s : pingos_q;
            im = GenImageColor(48, 6, BLANK);
            lin(&im, 0, 47, 0, claro);
            lin(&im, 0, 47, 1, base);
            lin(&im, 0, 47, 2, base);
            for (int i = 0; i < 4; i++){
                for (int y = 3; y < pingos[i][2]; y++){
                    lin(&im, pingos[i][0], pingos[i][1], y, base);
                }
                lin(&im, pingos[i][0] + 1, pingos[i][1] - 1, pingos[i][2], sombra);
            }
            if (suico){
                lin(&im, 9, 10, 1, sombra);
                lin(&im, 23, 24, 2, sombra);
                lin(&im, 35, 36, 1, sombra);
            }
        } break;

        case ING_CEBOLA: {
            Color roxo = rgb(164,112,186), lilas = rgb(214,172,224), claro = rgb(248,236,250);
            im = GenImageColor(48, 3, BLANK);
            lin(&im, 4, 43, 0, roxo);
            lin(&im, 4, 43, 2, roxo);
            for (int x = 3; x <= 44; x++){
                ImageDrawPixel(&im, x, 1, (x % 2) ? claro : lilas);
            }
            ImageDrawPixel(&im, 3, 1, roxo);
            ImageDrawPixel(&im, 44, 1, roxo);
        } break;

        case ING_TOMATE: {
            Color claro = rgb(240,104,92), base = rgb(214,56,50), brilho = rgb(250,166,154), escuro = rgb(166,34,32);
            im = GenImageColor(48, 5, BLANK);
            for (int i = 0; i < 4; i++){
                int x0 = 1 + i * 12;
                lin(&im, x0 + 1, x0 + 8, 0, claro);
                for (int y = 1; y <= 3; y++){
                    lin(&im, x0, x0 + 9, y, base);
                }
                lin(&im, x0 + 2, x0 + 4, 1, brilho);
                lin(&im, x0 + 1, x0 + 8, 4, escuro);
            }
        } break;

        case ING_CARNE: {
            Color base = rgb(112,62,32), escuro = rgb(80,42,20), pinta = rgb(66,34,16), claro = rgb(146,86,46);
            im = GenImageColor(48, 10, BLANK);
            lin(&im, 2, 45, 0, escuro);
            lin(&im, 1, 46, 1, base);
            for (int y = 2; y <= 7; y++){
                lin(&im, 0, 47, y, base);
                ImageDrawPixel(&im, 0, y, escuro);
                ImageDrawPixel(&im, 47, y, escuro);
            }
            lin(&im, 1, 46, 8, base);
            lin(&im, 2, 45, 9, escuro);
            ImageDrawPixel(&im, 1, 1, escuro);
            ImageDrawPixel(&im, 46, 1, escuro);
            ImageDrawPixel(&im, 1, 8, escuro);
            ImageDrawPixel(&im, 46, 8, escuro);
            // pintinhas espalhadas (sorteio fixo, sempre igual)
            unsigned int sorteio = 7u;
            for (int i = 0; i < 40; i++){
                sorteio = sorteio * 1103515245u + 12345u;
                int x = 2 + (int) ((sorteio >> 16) % 44u);
                sorteio = sorteio * 1103515245u + 12345u;
                int y = 2 + (int) ((sorteio >> 16) % 6u);
                ImageDrawPixel(&im, x, y, (i % 4 == 0) ? claro : pinta);
            }
        } break;

        case ING_OVO_FRITO: {
            static const int clara[6] = { 8, 4, 2, 2, 4, 8 };
            Color branco = rgb(250,250,244), borda = rgb(212,208,196);
            Color gema = rgb(246,192,32), gema_clara = rgb(252,226,100), gema_esc = rgb(222,154,18);
            im = GenImageColor(48, 9, BLANK);
            for (int i = 0; i < 6; i++){
                int y = 3 + i, e = clara[i], d = 47 - clara[i];
                lin(&im, e, d, y, branco);
                ImageDrawPixel(&im, e, y, borda);
                ImageDrawPixel(&im, d, y, borda);
                if (i > 0 && clara[i - 1] > clara[i]){
                    lin(&im, e, clara[i - 1], y, borda);
                    lin(&im, 47 - clara[i - 1], d, y, borda);
                }
                if (i < 5 && clara[i + 1] > clara[i]){
                    lin(&im, e, clara[i + 1], y, borda);
                    lin(&im, 47 - clara[i + 1], d, y, borda);
                }
            }
            lin(&im, clara[0], 47 - clara[0], 3, borda);
            lin(&im, clara[5], 47 - clara[5], 8, borda);
            lin(&im, 20, 27, 0, gema);
            lin(&im, 18, 29, 1, gema);
            lin(&im, 17, 30, 2, gema);
            lin(&im, 17, 30, 3, gema);
            lin(&im, 18, 29, 4, gema_esc);
            lin(&im, 20, 27, 5, gema_esc);
            lin(&im, 19, 21, 1, gema_clara);
            lin(&im, 18, 19, 2, gema_clara);
        } break;

        case ING_BACON: {
            Color topo = rgb(186,50,48), rosa = rgb(244,180,160), vermelho = rgb(206,64,58), escuro = rgb(146,32,34);
            im = GenImageColor(48, 7, BLANK);
            for (int x = 0; x < 48; x++){
                int f  = x % 16;
                int dy = (f < 6) ? 0 : (f < 8) ? 1 : (f < 14) ? 2 : 1;
                ImageDrawPixel(&im, x, dy,     topo);
                ImageDrawPixel(&im, x, dy + 1, rosa);
                ImageDrawPixel(&im, x, dy + 2, vermelho);
                ImageDrawPixel(&im, x, dy + 3, vermelho);
                ImageDrawPixel(&im, x, dy + 4, escuro);
            }
        } break;

        default: { // pao de baixo
            Color base = rgb(224,155,70), claro = rgb(238,180,96), escuro = rgb(200,128,52), contorno = rgb(164,98,40);
            im = GenImageColor(48, 9, BLANK);
            lin(&im, 0, 47, 0, claro);
            for (int y = 1; y <= 5; y++){
                lin(&im, 0, 47, y, base);
                ImageDrawPixel(&im, 0, y, escuro);
                ImageDrawPixel(&im, 47, y, escuro);
            }
            lin(&im, 0, 47, 6, escuro);
            lin(&im, 1, 46, 7, escuro);
            ImageDrawPixel(&im, 1, 7, contorno);
            ImageDrawPixel(&im, 46, 7, contorno);
            lin(&im, 2, 45, 8, contorno);
        } break;
    }
    return im;
}

static void carregar_texturas(void){
    for (int i = 0; i < 13; i++){
        const char *caminho = TextFormat("assets/%s.png", ARQUIVOS_CAMADA[i]);
        Image im;
        if (FileExists(caminho)){
            im = LoadImage(caminho);
            ImageAlphaCrop(&im, 0.0f);   // tira a borda transparente pra empilhar certinho
        }else{
            im = gerar_camada(CAMADAS[i]);
        }
        g_tex[i] = LoadTextureFromImage(im);
        UnloadImage(im);
    }
}

static void descarregar_texturas(void){
    for (int i = 0; i < 13; i++){
        UnloadTexture(g_tex[i]);
    }
}

static int indice_camada(char s){
    int i = posicao(CAMADAS, s);
    if (i < 0){
        return IDX_PAO_BAIXO;
    }
    return i;
}

// quanto (em pixels da arte) a camada encosta por cima da de baixo
static int sobreposicao(char s){
    switch (s){
        case ING_QUEIJO_SUICO:   
        case ING_QUEIJO_CHEDDAR: 
        case ING_ALFACE:         
            return 3;   // os pingos e as pontas caem por cima
        case ING_PAO_CIMA:       
            return 2;
    }
    return 1;
}

// desenha o hamburguer de baixo pra cima, o pao de baixo sempre vem primeiro
// camadas sao os simbolos acima dele; meio_y/dir_x (opcionais) recebem a posicao de cada camada
static void desenhar_hamburguer(const char *camadas, float cx, float base_y, float largura,
                                float altura_max, float *meio_y, float *dir_x){
    Texture2D pao = g_tex[IDX_PAO_BAIXO];
    float px_arte = (float) pao.width / 48.0f;       // 1 pixel da arte em pixels da textura
    float esc = largura / (float) pao.width;
    int n = (int) strlen(camadas);

    float total = (float) pao.height;
    for (int i = 0; i < n; i++){
        char s = camadas[i];
        total += (float) g_tex[indice_camada(s)].height - (float) sobreposicao(s) * px_arte;
    }
    if (total * esc > altura_max){
        esc = altura_max / total;
    }
    if (esc >= 1.0f){
        esc = floorf(esc);   // escala inteira deixa a pixel art nitida
    }

    float topo = base_y;
    for (int i = -1; i < n; i++){
        Texture2D t = (i < 0) ? pao : g_tex[indice_camada(camadas[i])];
        float w = (float) t.width * esc;
        float h = (float) t.height * esc;
        float fundo = base_y;
        if (i >= 0){
            fundo = topo + (float) sobreposicao(camadas[i]) * px_arte * esc;
        }
        Rectangle dst = { roundf(cx - w / 2.0f), roundf(fundo - h), w, h };
        DrawTexturePro(t, (Rectangle){ 0, 0, (float) t.width, (float) t.height }, dst,
                       (Vector2){ 0, 0 }, 0.0f, WHITE);
        if (i >= 0 && meio_y != NULL){
            meio_y[i] = dst.y + h / 2.0f;
        }
        if (i >= 0 && dir_x != NULL){
            dir_x[i] = dst.x + w;
        }
        topo = dst.y;
    }
}

/* ---------- utilidades de desenho ---------- */

static void texto_centro(const char *t, float cx, float y, int tam, Color c){
    DrawText(t, (int) (cx - (float) MeasureText(t, tam) / 2.0f), (int) y, tam, c);
}

static void texto_direita(const char *t, float xd, float y, int tam, Color c){
    DrawText(t, (int) (xd - (float) MeasureText(t, tam)), (int) y, tam, c);
}

// retangulo arredondado com borda
static void painel(Rectangle r, float raio, Color fundo, Color borda){
    float menor = (r.width < r.height) ? r.width : r.height;
    DrawRectangleRounded(r, 2.0f * raio / menor, 8, borda);
    Rectangle dentro = { r.x + 2, r.y + 2, r.width - 4, r.height - 4 };
    DrawRectangleRounded(dentro, 2.0f * (raio - 2) / (menor - 4), 8, fundo);
}

static Color cor_ingrediente(char s){
    switch (s){
        case ING_PAO_CIMA:       return rgb(224,155,70);
        case ING_KETCHUP:        return rgb(214,52,44);
        case ING_MOSTARDA:       return rgb(240,196,40);
        case ING_PICLES:         return rgb(98,158,58);
        case ING_ALFACE:         return rgb(122,206,82);
        case ING_QUEIJO_SUICO:   return rgb(246,224,128);
        case ING_QUEIJO_CHEDDAR: return rgb(246,166,34);
        case ING_CEBOLA:         return rgb(214,170,222);
        case ING_TOMATE:         return rgb(216,58,52);
        case ING_CARNE:          return rgb(112,62,32);
        case ING_OVO_FRITO:      return rgb(250,250,244);
        case ING_BACON:          return rgb(204,64,58);
    }
    return GRAY;
}

// texto preto em cor clara, branco em cor escura
static Color cor_texto_sobre(Color c){
    int lum = (c.r * 299 + c.g * 587 + c.b * 114) / 1000;
    return (lum > 150) ? COR_TEXTO : RAYWHITE;
}

static Color escurecer(Color c, float f){
    return (Color){ (unsigned char) (c.r * f), (unsigned char) (c.g * f), (unsigned char) (c.b * f), c.a };
}

// quebra o texto em linhas que cabem na largura, devolve quantas linhas
static int quebrar_texto(const char *texto, int tam, float largura, char linhas[][160], int max){
    int n = 0;
    char atual[160] = "";
    const char *p = texto;
    while (*p != '\0' && n < max){
        char palavra[160];
        int k = 0;
        while (*p == ' '){
            p++;
        }
        while (*p != '\0' && *p != ' ' && k < 159){
            palavra[k++] = *p++;
        }
        palavra[k] = '\0';
        if (k == 0){
            break;
        }
        char teste[320];
        if (atual[0] != '\0'){
            snprintf(teste, sizeof teste, "%s %s", atual, palavra);
        }else{
            snprintf(teste, sizeof teste, "%s", palavra);
        }
        if (atual[0] != '\0' && (float) MeasureText(teste, tam) > largura){
            snprintf(linhas[n++], 160, "%s", atual);
            snprintf(atual, sizeof atual, "%s", palavra);
        }else{
            snprintf(atual, sizeof atual, "%.159s", teste);
        }
    }
    if (atual[0] != '\0' && n < max){
        snprintf(linhas[n++], 160, "%s", atual);
    }
    return n;
}

/* ---------- mudancas da cena (chamadas pelo jogo, so visuais) ---------- */

static void avisar(const char *texto, Color cor){
    snprintf(g_cena.aviso, sizeof g_cena.aviso, "%s", texto);
    g_cena.cor_aviso = cor;
    g_cena.t_aviso = GetTime();
}

static void mover_jogador(Posicao p){
    g_cena.pos_anterior = g_cena.pos;
    g_cena.pos = p;
    g_cena.t_mov = GetTime();
}

static void cena_iniciar(const Fila *fila){
    memset(&g_cena, 0, sizeof g_cena);
    g_cena.fila = fila;
    g_cena.na_fila = fila->tamanho;
    g_cena.pos = POS_INICIO;
    g_cena.pos_anterior = POS_INICIO;
    g_cena.t_mov = -10.0;
    g_cena.t_avanco = -10.0;
    g_cena.t_aviso = -10.0;
    g_cena.resultado = RES_JOGANDO;
    carregar_texturas();
}

// cliente atendido: ele vai pra uma mesa (ou some) e a fila anda
static void cena_entregar(int atendidos){
    g_cena.atendidos = atendidos;
    g_cena.na_fila = g_cena.fila->tamanho;
    g_cena.t_avanco = GetTime();
    mover_jogador(POS_ENTREGA);
    if (g_cena.na_fila > 0){
        avisar("Pedido entregue! Vem o próximo cliente.", COR_OK);
    }else{
        avisar("Pedido entregue!", COR_OK);
    }
}

/* ---------- desenho do mapa ---------- */

static void desenhar_cliente(float cx, float cy, float tam, Color cor){
    Rectangle r = { cx - tam / 2.0f, cy - tam / 2.0f, tam, tam };
    DrawRectangleRec(r, escurecer(cor, 0.7f));
    DrawRectangleRec((Rectangle){ r.x + 3, r.y + 3, tam - 6, tam - 6 }, cor);
}

static void desenhar_triangulo(Vector2 c, float tam, Color cor, Color borda){
    float b = 3.0f;
    DrawTriangle((Vector2){ c.x, c.y - tam - b }, (Vector2){ c.x - tam - b, c.y + tam * 0.75f + b * 0.6f },
                 (Vector2){ c.x + tam + b, c.y + tam * 0.75f + b * 0.6f }, borda);
    DrawTriangle((Vector2){ c.x, c.y - tam }, (Vector2){ c.x - tam, c.y + tam * 0.75f },
                 (Vector2){ c.x + tam, c.y + tam * 0.75f }, cor);
}

static void desenhar_salao(void){
    DrawRectangle(RX, 0, 600, ALTURA_TELA, COR_PISO_SALAO);

    // mesas e cadeiras
    for (int i = 0; i < 4; i++){
        Rectangle m = MESAS[i];
        DrawRectangleRec(m, COR_MESA_ESC);
        DrawRectangleRec((Rectangle){ m.x + 4, m.y + 4, m.width - 8, m.height - 8 }, COR_MESA);
    }
    for (int i = 0; i < NUM_ASSENTOS; i++){
        Rectangle a = ASSENTOS[i];
        DrawRectangleRec(a, COR_MESA_ESC);
        DrawRectangleRec((Rectangle){ a.x + 3, a.y + 3, a.width - 6, a.height - 6 }, COR_MESA);
    }

    // quem ja foi atendido senta, se nao tiver cadeira so some
    int sentados = g_cena.atendidos;
    if (sentados > NUM_ASSENTOS){
        sentados = NUM_ASSENTOS;
    }
    for (int i = 0; i < sentados; i++){
        Rectangle a = ASSENTOS[i];
        desenhar_cliente(a.x + a.width / 2.0f, a.y + a.height / 2.0f, 28, COR_CLIENTE);
    }

    // fila: o da frente fica na mesa de entrega, os outros pra cima
    float desloc = 0.0f;
    double dt = GetTime() - g_cena.t_avanco;
    if (dt >= 0.0 && dt < 0.45){
        float k = (float) (dt / 0.45);
        desloc = (1.0f - k) * (1.0f - k) * FILA_PASSO;
    }
    int visiveis = g_cena.na_fila;
    if (visiveis > FILA_VISIVEL){
        visiveis = FILA_VISIVEL;
    }
    for (int i = visiveis - 1; i >= 0; i--){
        float cx = RX + 300;
        float cy = FILA_Y - (float) i * FILA_PASSO - desloc;
        Color cor = COR_CLIENTE;
        if (i == 0 && g_cena.paciencia <= 0){
            cor = COR_BRAVO;
        }
        desenhar_cliente(cx, cy, 34, cor);
    }
    if (g_cena.na_fila > FILA_VISIVEL){
        DrawText(TextFormat("+%d", g_cena.na_fila - FILA_VISIVEL), RX + 322,
                 (int) (FILA_Y - (FILA_VISIVEL - 1) * FILA_PASSO - 10), 20, COR_SUAVE);
    }

    // paciencia do cliente da frente
    if (g_cena.na_fila > 0){
        float frac = (float) g_cena.paciencia / (float) CFG_PACIENCIA_MAXIMA;
        if (frac < 0.0f) frac = 0.0f;
        if (frac > 1.0f) frac = 1.0f;
        Color cb = (frac > 0.5f) ? COR_OK : (frac > 0.25f) ? COR_MONTAGEM : COR_BRAVO;
        float y = FILA_Y - desloc;
        Rectangle barra = { RX + 326, y - 5, 64, 10 };
        DrawRectangleRec(barra, COR_TEXTO);
        DrawRectangle((int) barra.x + 2, (int) barra.y + 2, (int) ((barra.width - 4) * frac), 6, cb);
        DrawText(TextFormat("%d", g_cena.paciencia), (int) (barra.x + barra.width + 6), (int) (y - 10), 20, COR_TEXTO);
    }
}

static void desenhar_chapa(void){
    Rectangle r = R_CHAPA;
    DrawRectangleRec(r, COR_CHAPA);
    Rectangle in = { r.x + 5, r.y + 5, r.width - 10, r.height - 10 };
    DrawRectangleRec(in, COR_CHAPA_FUNDO);

    // uma faixa pra cada ingrediente que precisa de preparo
    static const char ordem[] = "sqctnob";
    float lw = in.width / 7.0f;
    for (int i = 0; i < 7; i++){
        float x = in.x + (float) i * lw;
        if (i > 0){
            DrawRectangle((int) x - 1, (int) in.y, 2, (int) in.height, COR_CHAPA);
        }
        char rotulo[2] = { ordem[i], '\0' };
        texto_centro(rotulo, x + lw / 2.0f, in.y + 2, 10, (Color){ 236, 196, 186, 255 });

        int perecivel = (i >= 4);
        int qtd = perecivel ? g_cena.qtd_pereciveis[i - 4] : g_cena.perenes[i];
        int vis = (qtd > 4) ? 3 : qtd;
        Color cor = cor_ingrediente(ordem[i]);
        float bw = lw - 8.0f, bh = 13.0f;
        for (int k = 0; k < vis; k++){
            Rectangle b = { x + 4.0f, in.y + in.height - 3.0f - (float) (k + 1) * (bh + 2.0f), bw, bh };
            DrawRectangleRec(b, escurecer(cor, 0.75f));
            DrawRectangleRec((Rectangle){ b.x + 1, b.y + 1, b.width - 2, b.height - 2 }, cor);
            if (ordem[i] == ING_OVO_FRITO){
                DrawRectangle((int) (b.x + b.width / 2) - 3, (int) b.y + 3, 6, 7, rgb(246,192,32));
            }
            if (perecivel){
                // quantos turnos faltam pra estragar
                int falta = g_cena.validades[i - 4][k] - g_cena.turno;
                Color ct = cor_texto_sobre(cor);
                float xt = (ordem[i] == ING_OVO_FRITO) ? b.x + 3 : b.x + b.width / 2.0f - 3;
                DrawText(TextFormat("%d", falta), (int) xt, (int) b.y + 2, 10, ct);
            }
        }
        if (qtd > 4){
            texto_centro(TextFormat("+%d", qtd - 3), x + lw / 2.0f, in.y + in.height - 3 - 4 * (bh + 2) + 2, 10, RAYWHITE);
        }
    }
}

static void desenhar_balcoes(void){
    // piso da cozinha em xadrez
    for (int y = 495, li = 0; y < ALTURA_TELA; y += 25, li++){
        for (int x = RX + 27, co = 0; x < RX + 573; x += 26, co++){
            DrawRectangle(x, y, 26, 25, ((li + co) % 2) ? COR_PISO_A : COR_PISO_B);
        }
    }

    desenhar_chapa();

    DrawRectangleRec(R_ENTREGA, COR_ENTREGA_ESC);
    DrawRectangleRec((Rectangle){ R_ENTREGA.x + 4, R_ENTREGA.y + 4, R_ENTREGA.width - 8, R_ENTREGA.height - 8 }, COR_ENTREGA);
    DrawRectangleRec((Rectangle){ R_ENTREGA.x + 25, R_ENTREGA.y + 30, R_ENTREGA.width - 50, 34 },
                     escurecer(COR_ENTREGA, 0.85f));   // bandeja

    DrawRectangleRec(R_MONTAGEM, COR_MONTAGEM_ESC);
    DrawRectangleRec((Rectangle){ R_MONTAGEM.x + 4, R_MONTAGEM.y + 4, R_MONTAGEM.width - 8, R_MONTAGEM.height - 8 }, COR_MONTAGEM);

    // o prato que esta sendo montado, pequeno, em cima da mesa de montagem
    float cx = R_MONTAGEM.x + R_MONTAGEM.width / 2.0f;
    float base = R_MONTAGEM.y + R_MONTAGEM.height - 10;
    DrawEllipse((int) cx, (int) base + 2, 36, 6, COR_PRATO_ESC);
    DrawEllipse((int) cx, (int) base, 34, 5, COR_PRATO);
    desenhar_hamburguer(g_cena.atual, cx, base, 48, R_MONTAGEM.height - 16, NULL, NULL);

    // destaque no movel onde o jogador esta
    Rectangle alvo = { 0, 0, 0, 0 };
    if (g_cena.pos == POS_CHAPA)    alvo = R_CHAPA;
    if (g_cena.pos == POS_ENTREGA)  alvo = R_ENTREGA;
    if (g_cena.pos == POS_MONTAGEM) alvo = R_MONTAGEM;
    if (alvo.width > 0){
        float a = 0.55f + 0.35f * sinf((float) GetTime() * 5.0f);
        DrawRectangleLinesEx(alvo, 3, Fade(RAYWHITE, a));
    }

    // nomes no chao
    texto_centro("CHAPA",    R_CHAPA.x + R_CHAPA.width / 2.0f,       499, 10, COR_SUAVE);
    texto_centro("ENTREGA",  R_ENTREGA.x + R_ENTREGA.width / 2.0f,   499, 10, COR_SUAVE);
    texto_centro("MONTAGEM", R_MONTAGEM.x + R_MONTAGEM.width / 2.0f, 499, 10, COR_SUAVE);

    // lixeira
    DrawCircleV(P_LIXEIRA, 27, COR_LIXEIRA_ESC);
    DrawCircleV(P_LIXEIRA, 23, COR_LIXEIRA);
    DrawRectangle((int) P_LIXEIRA.x - 12, (int) P_LIXEIRA.y - 3, 24, 6, COR_LIXEIRA_ESC);
    if (g_cena.pos == POS_LIXEIRA){
        float a = 0.55f + 0.35f * sinf((float) GetTime() * 5.0f);
        DrawCircleLines((int) P_LIXEIRA.x, (int) P_LIXEIRA.y, 30, Fade(COR_LIXEIRA_ESC, a));
    }
    texto_centro("LIXEIRA", P_LIXEIRA.x, P_LIXEIRA.y - 42, 10, COR_SUAVE);
}

// parede baixa de fast food
static void desenhar_paredes(void){
    DrawRectangleRounded((Rectangle){ RX + 13,  393, 237, 14 }, 1.0f, 6, COR_PAREDE);
    DrawRectangleRounded((Rectangle){ RX + 350, 393, 237, 14 }, 1.0f, 6, COR_PAREDE);
    DrawRectangle(RX + 13,  400, 14, ALTURA_TELA - 400, COR_PAREDE);
    DrawRectangle(RX + 573, 400, 14, ALTURA_TELA - 400, COR_PAREDE);
}

static void desenhar_jogador(void){
    Vector2 p = POSICOES[g_cena.pos];
    double dt = GetTime() - g_cena.t_mov;
    if (dt >= 0.0 && dt < 0.35 && g_cena.pos_anterior != g_cena.pos){
        float k = (float) (dt / 0.35);
        // rastro no lugar antigo e um anel onde apareceu (o "teleporte")
        Vector2 antes = POSICOES[g_cena.pos_anterior];
        desenhar_triangulo(antes, 18.0f * (1.0f - k), Fade(COR_JOGADOR, 0.5f * (1.0f - k)), Fade(COR_JOGADOR_ESC, 0.5f * (1.0f - k)));
        DrawRing(p, 10.0f + 28.0f * k, 14.0f + 28.0f * k, 0.0f, 360.0f, 32, Fade(COR_JOGADOR, 1.0f - k));
    }
    desenhar_triangulo(p, 18.0f, COR_JOGADOR, COR_JOGADOR_ESC);
}

static void desenhar_aviso(void){
    if (g_cena.aviso[0] == '\0'){
        return;
    }
    double dt = GetTime() - g_cena.t_aviso;
    if (dt < 0.0 || dt > 4.0){
        return;
    }
    float alfa = (dt < 3.4) ? 1.0f : (float) ((4.0 - dt) / 0.6);
    char linhas[3][160];
    int n = quebrar_texto(g_cena.aviso, 20, 540, linhas, 3);
    float w = 0;
    for (int i = 0; i < n; i++){
        float wl = (float) MeasureText(linhas[i], 20);
        if (wl > w) w = wl;
    }
    Rectangle r = { RX + 300 - (w + 32) / 2.0f, 12, w + 32, (float) n * 24 + 14 };
    DrawRectangleRounded(r, 0.35f, 8, Fade(g_cena.cor_aviso, 0.94f * alfa));
    for (int i = 0; i < n; i++){
        texto_centro(linhas[i], RX + 300, r.y + 9 + (float) i * 24, 20, Fade(RAYWHITE, alfa));
    }
}

/* ---------- paineis ---------- */

static void desenhar_coluna_hamburguer(Rectangle r, const char *titulo, const char *camadas){
    painel(r, 10, COR_PAINEL, COR_BORDA);
    texto_centro("HAMBÚRGUER", r.x + r.width / 2.0f, r.y + 14, 20, COR_SUAVE);
    texto_centro(titulo, r.x + r.width / 2.0f, r.y + 38, 20, COR_TEXTO);

    float cx = r.x + 6 + LARGURA_PAO / 2.0f;
    float base = r.y + r.height - 40;
    DrawEllipse((int) cx, (int) base + 3, 82, 11, COR_PRATO_ESC);
    DrawEllipse((int) cx, (int) base, 80, 9, COR_PRATO);

    float meio[MAX_CAMADAS], dir[MAX_CAMADAS];
    desenhar_hamburguer(camadas, cx, base, LARGURA_PAO, base - (r.y + 72), meio, dir);

    // letra de cada camada do lado direito (empurra pra cima se encostar)
    float x_tag = r.x + 156;
    float ultimo = 100000.0f;
    int n = (int) strlen(camadas);
    for (int i = 0; i < n; i++){
        float y = meio[i] - 10.0f;
        if (y > ultimo - 19.0f){
            y = ultimo - 19.0f;
        }
        ultimo = y;
        DrawLineEx((Vector2){ dir[i] + 2, meio[i] }, (Vector2){ x_tag - 2, y + 10 }, 1.0f, COR_BORDA);
        char s[2] = { camadas[i], '\0' };
        DrawText(s, (int) x_tag, (int) y, 20, COR_TEXTO);
    }
}

static void desenhar_paineis(void){
    desenhar_coluna_hamburguer((Rectangle){ 8,   8, 172, 704 }, "ATUAL",    g_cena.atual);
    desenhar_coluna_hamburguer((Rectangle){ 186, 8, 172, 704 }, "DESEJADO", g_cena.desejado);
}

// caixa onde aparece o que o jogador esta digitando
static void desenhar_caixa_texto(Rectangle r, const char *buf){
    painel(r, 8, WHITE, COR_LARANJA);
    const char *p = buf;
    float max = r.width - 50;
    while (*p != '\0' && (float) MeasureText(p, 20) > max){
        p++;
        while (((unsigned char) *p & 0xC0) == 0x80){
            p++;
        }
    }
    float ty = r.y + (r.height - 20) / 2.0f;
    DrawText(">", (int) r.x + 12, (int) ty, 20, COR_LARANJA);
    DrawText(p, (int) r.x + 32, (int) ty, 20, COR_TEXTO);
    if (fmod(GetTime(), 1.0) < 0.55){
        DrawRectangle((int) r.x + 34 + MeasureText(p, 20), (int) ty, 10, 20, COR_LARANJA);
    }
}

static void desenhar_hud(void){
    Rectangle r = { 975, 8, 297, 704 };
    painel(r, 10, COR_PAINEL, COR_BORDA);
    float x = r.x + 16, xd = r.x + r.width - 16;

    texto_centro("DADO BURGER", r.x + r.width / 2.0f, 20, 30, COR_LARANJA);
    DrawText(TextFormat("Cliente %d de %d", g_cena.numero, CFG_NUM_CLIENTES), (int) x, 62, 20, COR_TEXTO);
    DrawText(TextFormat("Turno %d", g_cena.turno), (int) x, 86, 20, COR_TEXTO);
    DrawText(TextFormat("Pedido nº %d", g_cena.codigo), (int) x, 110, 20, COR_SUAVE);

    DrawText("Paciência", (int) x, 136, 20, COR_TEXTO);
    float frac = (float) g_cena.paciencia / (float) CFG_PACIENCIA_MAXIMA;
    if (frac < 0.0f) frac = 0.0f;
    if (frac > 1.0f) frac = 1.0f;
    Color cb = (frac > 0.5f) ? COR_OK : (frac > 0.25f) ? COR_MONTAGEM : COR_BRAVO;
    Rectangle barra = { x + 118, 136, xd - x - 118, 20 };
    DrawRectangleRec(barra, COR_TEXTO);
    DrawRectangle((int) barra.x + 2, (int) barra.y + 2, (int) ((barra.width - 4) * frac), 16, cb);
    texto_centro(TextFormat("%d / %d", g_cena.paciencia, CFG_PACIENCIA_MAXIMA), barra.x + barra.width / 2.0f, 141, 10, RAYWHITE);

    DrawRectangle((int) x, 166, (int) (xd - x), 2, COR_BORDA);

    // ingredientes com preparo (estoque) e sem preparo
    DrawText("COM PREPARO", (int) x, 174, 20, COR_LARANJA);
    texto_direita("estoque", xd, 180, 10, COR_SUAVE);
    static const char com_preparo[] = "sqctnob";
    for (int i = 0; i < 7; i++){
        char s = com_preparo[i];
        float y = 200.0f + (float) i * 22.0f;
        int qtd = (i < 4) ? g_cena.perenes[i] : g_cena.qtd_pereciveis[i - 4];
        Color c = cor_ingrediente(s);
        DrawRectangle((int) x, (int) y + 2, 16, 16, escurecer(c, 0.7f));
        DrawRectangle((int) x + 2, (int) y + 4, 12, 12, c);
        char sim[2] = { s, '\0' };
        DrawText(sim, (int) x + 24, (int) y, 20, COR_TEXTO);
        DrawText(nome_ingrediente(s), (int) x + 44, (int) y, 20, COR_TEXTO);
        if (i >= 4){
            DrawText("perece", (int) (x + 48 + MeasureText(nome_ingrediente(s), 20)), (int) y + 6, 10, COR_SUAVE);
        }
        texto_direita(TextFormat("%d", qtd), xd, y, 20, (qtd > 0) ? COR_TEXTO : COR_SUAVE);
    }

    DrawText("SEM PREPARO", (int) x, 358, 20, COR_LARANJA);
    texto_direita("infinito", xd, 364, 10, COR_SUAVE);
    for (int i = 0; i < 5; i++){
        char s = INFINITOS[i];
        float y = 384.0f + (float) i * 22.0f;
        Color c = cor_ingrediente(s);
        DrawRectangle((int) x, (int) y + 2, 16, 16, escurecer(c, 0.7f));
        DrawRectangle((int) x + 2, (int) y + 4, 12, 12, c);
        char sim[2] = { s, '\0' };
        DrawText(sim, (int) x + 24, (int) y, 20, COR_TEXTO);
        DrawText(nome_ingrediente(s), (int) x + 44, (int) y, 20, COR_TEXTO);
    }

    DrawRectangle((int) x, 498, (int) (xd - x), 2, COR_BORDA);

    DrawText("COMANDOS", (int) x, 506, 20, COR_LARANJA);
    DrawText("p <ing>", (int) x, 532, 20, COR_TEXTO);  DrawText("preparar",        (int) x + 104, 532, 20, COR_SUAVE);
    DrawText("a <ing>", (int) x, 554, 20, COR_TEXTO);  DrawText("adicionar",       (int) x + 104, 554, 20, COR_SUAVE);
    DrawText("r",       (int) x, 576, 20, COR_TEXTO);  DrawText("remover o topo",  (int) x + 104, 576, 20, COR_SUAVE);
    DrawText("s",       (int) x, 598, 20, COR_TEXTO);  DrawText("sair da partida", (int) x + 104, 598, 20, COR_SUAVE);

    DrawRectangle((int) x, 626, (int) (xd - x), 2, COR_BORDA);
}

static void desenhar_cena(void){
    ClearBackground(COR_FUNDO);
    desenhar_paineis();
    desenhar_salao();
    desenhar_balcoes();
    desenhar_paredes();
    desenhar_jogador();
    desenhar_hud();
    desenhar_aviso();
}

static void desenhar_entrada_comando(const char *buf){
    float x = 991, xd = 1256;
    DrawText("SEU COMANDO", (int) x, 634, 10, COR_SUAVE);
    texto_direita("Enter envia, Esc limpa", xd, 634, 10, COR_SUAVE);
    desenhar_caixa_texto((Rectangle){ x, 648, xd - x, 54 }, buf);
}

// fim da partida: resultado, pontuacao e o nome do jogador
static void desenhar_fim(const char *buf){
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.55f));
    Rectangle r = { 340, 170, 600, 360 };
    painel(r, 14, COR_PAINEL, COR_LARANJA);
    float cx = r.x + r.width / 2.0f;

    const char *titulo = "VITÓRIA!";
    const char *sub = "Todos os clientes foram atendidos!";
    Color cor = COR_OK;
    if (g_cena.resultado == RES_DERROTA){
        titulo = "DERROTA";
        sub = "Um cliente perdeu a paciência.";
        cor = COR_ERRO;
    }else if (g_cena.resultado == RES_SAIU){
        titulo = "FIM DE JOGO";
        sub = "Você saiu da partida.";
        cor = COR_SUAVE;
    }
    texto_centro(titulo, cx, r.y + 30, 40, cor);
    texto_centro(sub, cx, r.y + 84, 20, COR_TEXTO);
    texto_centro(TextFormat("Pontuação: %d", g_cena.atendidos), cx, r.y + 130, 30, COR_LARANJA);

    DrawText("Nome do jogador:", (int) r.x + 60, (int) r.y + 196, 20, COR_TEXTO);
    desenhar_caixa_texto((Rectangle){ r.x + 60, r.y + 222, r.width - 120, 54 }, buf);
    texto_centro("Digite seu nome e aperte Enter", cx, r.y + 300, 20, COR_SUAVE);
}

/* ---------- teclado ---------- */

static double g_t_apagar = 0.0;

// poe o caractere digitado no fim do texto (em utf-8, igual o terminal faria)
static void anexar_codepoint(char *buf, int tam, int cp){
    int n = (int) strlen(buf);
    if (cp < 128){
        if (n + 1 >= tam){
            return;
        }
        buf[n++] = (char) cp;
    }else{
        if (n + 2 >= tam){
            return;
        }
        buf[n++] = (char) (0xC0 | (cp >> 6));
        buf[n++] = (char) (0x80 | (cp & 0x3F));
    }
    buf[n] = '\0';
}

static void apagar_ultimo(char *buf){
    int n = (int) strlen(buf);
    while (n > 0 && ((unsigned char) buf[n - 1] & 0xC0) == 0x80){
        n--;
    }
    if (n > 0){
        n--;
    }
    buf[n] = '\0';
}

// le o que foi digitado neste frame, devolve 1 quando aperta enter
// (usa a fila de teclas do raylib pra nao perder tecla apertada e solta no mesmo frame)
static int teclado_ler(char *buf, int tam){
    int enter = 0;
    double agora = GetTime();
    int cp = GetCharPressed();
    while (cp > 0){
        if ((cp >= 32 && cp < 127) || (cp >= 160 && cp <= 255)){
            anexar_codepoint(buf, tam, cp);
        }
        cp = GetCharPressed();
    }
    int tecla = GetKeyPressed();
    while (tecla != 0){
        if (tecla == KEY_ENTER || tecla == KEY_KP_ENTER){
            enter = 1;
        }else if (tecla == KEY_BACKSPACE){
            apagar_ultimo(buf);
            g_t_apagar = agora + 0.4;
        }else if (tecla == KEY_ESCAPE){
            buf[0] = '\0';
        }
        tecla = GetKeyPressed();
    }
    if (IsKeyDown(KEY_BACKSPACE) && agora >= g_t_apagar){
        apagar_ultimo(buf);   // segurando o backspace
        g_t_apagar = agora + 0.04;
    }
    return enter;
}

// so pra telas que esperam enter/esc pra sair
static int apertou_enter_ou_esc(void){
    int sim = 0;
    int tecla = GetKeyPressed();
    while (tecla != 0){
        if (tecla == KEY_ENTER || tecla == KEY_KP_ENTER || tecla == KEY_ESCAPE){
            sim = 1;
        }
        tecla = GetKeyPressed();
    }
    return sim;
}

// faz o papel do fgets: desenha a tela ate o jogador apertar enter
// retorna 0 se a janela foi fechada (que e o "fim da entrada")
static int ler_linha(char *buf, int tam){
    int limite = tam;
    if (g_cena.resultado != RES_JOGANDO && limite > TAM_MAX_NOME + 1){
        limite = TAM_MAX_NOME + 1;   // o historico ja guarda so 20 caracteres do nome
    }
    buf[0] = '\0';
    while (!WindowShouldClose()){
        BeginDrawing();
        desenhar_cena();
        if (g_cena.resultado == RES_JOGANDO){
            desenhar_entrada_comando(buf);
        }else{
            desenhar_fim(buf);
        }
        EndDrawing();
        // le depois de desenhar pra nao pegar o enter da tela anterior
        if (teclado_ler(buf, limite)){
            return 1;
        }
    }
    return 0;
}

// mostra o relatorio final (o mesmo texto que vai pro terminal e pro log)
static void mostrar_relatorio(void){
    static char texto[TAM_BUFFER_RELATORIO];
    gerar_texto_relatorio(texto, TAM_BUFFER_RELATORIO);

    // separa em secoes: cada "-- titulo --" seguido das linhas dela
    const char *linhas[96];
    int n = 0;
    for (char *p = strtok(texto, "\n"); p != NULL && n < 96; p = strtok(NULL, "\n")){
        if (p[0] != '='){
            linhas[n++] = p;
        }
    }
    // da metade pra frente vai pra coluna da direita, sem partir uma secao no meio
    int metade = 0;
    for (int i = 0; i < n && metade == 0; i++){
        if (linhas[i][0] == '-' && linhas[i][1] == '-' && i >= n / 2){
            metade = i;
        }
    }

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(COR_FUNDO);
        painel((Rectangle){ 40, 24, 1200, 672 }, 14, COR_PAINEL, COR_BORDA);
        texto_centro("RELATÓRIO DE DESEMPENHO", LARGURA_TELA / 2.0f, 48, 40, COR_LARANJA);

        float col_x[2] = { 80, 670 };
        float col_w = 530;
        float y = 120;
        int col = 0;
        for (int i = 0; i < n; i++){
            if (i == metade && metade > 0){
                col = 1;
                y = 120;
            }
            const char *l = linhas[i];
            float x = col_x[col];
            if (l[0] == '-' && l[1] == '-'){
                if (y > 120){
                    y += 14;
                }
                char titulo[96];
                snprintf(titulo, sizeof titulo, "%s", l + 3);
                int k = (int) strlen(titulo);
                while (k > 0 && (titulo[k - 1] == '-' || titulo[k - 1] == ' ')){
                    titulo[--k] = '\0';
                }
                DrawText(titulo, (int) x, (int) y, 20, COR_LARANJA);
                DrawRectangle((int) x, (int) y + 23, (int) col_w, 2, COR_BORDA);
                y += 32;
                continue;
            }
            // "rotulo:   valor" vira duas colunas alinhadas
            const char *dois_pontos = strstr(l, ": ");
            if (dois_pontos != NULL){
                char rotulo[96];
                int k = (int) (dois_pontos - l) + 1;
                if (k > 95) k = 95;
                memcpy(rotulo, l, (size_t) k);
                rotulo[k] = '\0';
                const char *valor = dois_pontos + 1;
                while (*valor == ' '){
                    valor++;
                }
                DrawText(rotulo, (int) x, (int) y, 20, COR_TEXTO);
                texto_direita(valor, x + col_w, y, 20, COR_TEXTO);
            }else{
                DrawText(l, (int) x, (int) y, 20, COR_SUAVE);
            }
            y += 26;
        }

        texto_centro(TextFormat("O relatório também foi gravado em %s", CFG_ARQUIVO_LOG),
                     LARGURA_TELA / 2.0f, 610, 20, COR_SUAVE);
        texto_centro("Pressione Enter para voltar ao menu", LARGURA_TELA / 2.0f, 646, 20, COR_TEXTO);
        EndDrawing();

        if (apertou_enter_ou_esc() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            break;
        }
    }
}

/* ==========================================================================
 * JOGO (a logica e a mesma de antes, so os printf viraram coisas na tela)
 * ========================================================================== */

// tira do heap o que vence neste turno ou antes
static void descartar_vencidos(Estoque *e, int turno){
    for (int i = 0; i < 3; i++){
        while (e->pereciveis[i].tam > 0 && fpMinimo(&e->pereciveis[i]) <= turno){
            fpRemoverMinimo(&e->pereciveis[i]);
        }
    }
}

// copia o pedido, o prato e o estoque pra cena (so le, nao desempilha nada)
static void mostrar_tela(const Cliente *c, int numero, const Pilha *prato, const Estoque *e, int turno){
    g_cena.numero = numero;
    g_cena.turno = turno;
    g_cena.paciencia = c->paciencia;
    g_cena.codigo = c->codigo;
    g_cena.atendidos = numero - 1;
    g_cena.na_fila = g_cena.fila->tamanho;
    snprintf(g_cena.desejado, sizeof g_cena.desejado, "%s", c->receita);

    // a pilha vai do topo pra baixo, a tela precisa de baixo pra cima
    char topo_baixo[MAX_CAMADAS];
    int n = 0;
    for (const NoPilha *no = prato->topo; no != NULL && n < MAX_CAMADAS; no = no->prox){
        topo_baixo[n++] = no->simbolo;
    }
    for (int i = 0; i < n; i++){
        g_cena.atual[i] = topo_baixo[n - 1 - i];
    }
    g_cena.atual[n] = '\0';

    for (int i = 0; i < 4; i++){
        g_cena.perenes[i] = e->perenes[i];
    }
    for (int i = 0; i < 3; i++){
        int q = e->pereciveis[i].tam;
        if (q > CFG_HEAP_CAP){
            q = CFG_HEAP_CAP;
        }
        g_cena.qtd_pereciveis[i] = q;
        // copia e ordena so pra mostrar o que vence primeiro embaixo
        for (int k = 0; k < q; k++){
            int v = e->pereciveis[i].dados[k];
            int j = k;
            while (j > 0 && g_cena.validades[i][j - 1] > v){
                g_cena.validades[i][j] = g_cena.validades[i][j - 1];
                j--;
            }
            g_cena.validades[i][j] = v;
        }
    }
}

// retorna 1 se gastou turno
static int acao_preparar(Estoque *e, char s, int turno){
    int i = posicao(PERENES, s);
    if (i >= 0){
        e->perenes[i]++;
        return 1;
    }
    i = posicao(PERECIVEIS, s);
    if (i >= 0){
        fpInserir(&e->pereciveis[i], turno + VALIDADE[i]);
        return 1;
    }
    avisar("Esse ingrediente não existe ou não requer preparo.", COR_ERRO);
    return 0;
}

// retorna 1 se gastou turno
static int acao_adicionar(Estoque *e, Pilha *prato, char s){
    if (pilha_topo_e_z(prato)){
        avisar("O pão de cima já foi colocado.", COR_ERRO);
        return 0;
    }
    int cat = categoria(s);
    if (cat == CAT_INFINITO){
        pilha_empilhar(prato, s);
        return 1;
    }
    if (cat == CAT_PERENE){
        int i = posicao(PERENES, s);
        if (e->perenes[i] == 0){
            avisar(TextFormat("Sem %s no estoque.", nome_ingrediente(s)), COR_ERRO);
            return 0;
        }
        e->perenes[i]--;
        pilha_empilhar(prato, s);
        return 1;
    }
    if (cat == CAT_PERECIVEL){
        int i = posicao(PERECIVEIS, s);
        if (e->pereciveis[i].tam == 0){
            avisar(TextFormat("Sem %s no estoque.", nome_ingrediente(s)), COR_ERRO);
            return 0;
        }
        fpRemoverMinimo(&e->pereciveis[i]);
        pilha_empilhar(prato, s);
        return 1;
    }
    avisar("Ingrediente inválido.", COR_ERRO);
    return 0;
}

static int acao_remover(Pilha *prato){
    if (pilha_vazia(prato)){
        avisar("O hambúrguer está vazio.", COR_ERRO);
        return 0;
    }
    pilha_desempilhar(prato);
    return 1;
}

// uma partida inteira
void jogo_executar(void){
    pedidos_iniciar_partida();

    Fila fila;
    fila_inicializar(&fila);
    for (int i = 0; i < CFG_NUM_CLIENTES; i++){
        Cliente c;
        c.codigo = 0;
        c.receita[0] = '\0';
        c.paciencia = CFG_PACIENCIA_MAXIMA;
        fila_entrar(&fila, &c);
    }

    Estoque estoque;
    memset(estoque.perenes, 0, sizeof(estoque.perenes));
    for (int i = 0; i < 3; i++){
        estoque.pereciveis[i].tam = 0;
    }

    int turno = 1;
    int atendidos = 0;
    int derrota = 0;
    int saiu = 0;
    char linha[TAM_ENTRADA];

    cena_iniciar(&fila);   // visual: zera a tela e carrega os sprites

    while (!fila_vazia(&fila) && !derrota && !saiu){
        Cliente *cliente = fila_frente(&fila);

        Pedido ped;
        get_pedido(sortear_codigo_pedido(), &ped);
        cliente->codigo = ped.codigo;
        strcpy(cliente->receita, ped.receita);

        Lista receita;
        receita_para_lista(cliente->receita, &receita);
        Pilha prato;
        pilha_inicializar(&prato);

        int concluido = 0;
        while (!concluido && !derrota && !saiu){
            descartar_vencidos(&estoque, turno);

            int consumiu = 0;
            while (!consumiu && !saiu){
                mostrar_tela(cliente, atendidos + 1, &prato, &estoque, turno);
                if (!ler_linha(linha, TAM_ENTRADA)){
                    saiu = 1;
                    break;
                }
                char cmd = '\0', ing = '\0';
                sscanf(linha, " %c %c", &cmd, &ing);

                if (cmd == 'p'){
                    consumiu = acao_preparar(&estoque, ing, turno);
                    if (consumiu){   // visual: vai pra chapa
                        mover_jogador(POS_CHAPA);
                        avisar(TextFormat("Preparando %s na chapa.", nome_ingrediente(ing)), COR_INFO);
                    }
                }else if (cmd == 'a'){
                    consumiu = acao_adicionar(&estoque, &prato, ing);
                    if (consumiu){   // visual: vai pra mesa de montagem
                        mover_jogador(POS_MONTAGEM);
                        avisar(TextFormat("Adicionou %s.", nome_ingrediente(ing)), COR_INFO);
                    }
                }else if (cmd == 'r'){
                    consumiu = acao_remover(&prato);
                    if (consumiu){   // visual: vai pra lixeira
                        mover_jogador(POS_LIXEIRA);
                        avisar("Jogou o ingrediente do topo no lixo.", COR_INFO);
                    }
                }else if (cmd == 's'){
                    saiu = 1;
                }else{
                    avisar("Comando inválido.", COR_ERRO);
                }
            }
            if (saiu){
                break;
            }

            // fim da acao: confere o hamburguer, senao perde 1 de paciencia
            turno++;
            if (pilha_topo_e_z(&prato) && pilha_coincide_com_lista(&prato, &receita)){
                concluido = 1;
            }else{
                cliente->paciencia--;
                if (cliente->paciencia == 0){
                    derrota = 1;
                }
            }
            mostrar_tela(cliente, atendidos + 1, &prato, &estoque, turno);   // visual: atualiza a copia
        }

        pilha_destruir(&prato);
        destruir(&receita);
        if (concluido){
            atendidos++;
            fila_sair(&fila);
            cena_entregar(atendidos);   // visual: mesa de entrega, cliente senta e a fila anda
        }
    }

    if (derrota){
        g_cena.resultado = RES_DERROTA;
    }else if (saiu){
        g_cena.resultado = RES_SAIU;
    }else{
        g_cena.resultado = RES_VITORIA;
    }
    g_cena.atendidos = atendidos;   // pontuacao

    fila_destruir(&fila);

    // a tela de fim pede o nome
    if (!ler_linha(linha, TAM_ENTRADA)){
        linha[0] = '\0';
    }
    adicionar_historico(linha, atendidos);
    exibir_e_gravar_relatorio_final(CFG_ARQUIVO_LOG);

    mostrar_relatorio();
    descarregar_texturas();
}
