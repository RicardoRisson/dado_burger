#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "../include/pedidos.h"
#include "../include/historico.h"
#include "../include/ordenacao.h"
#include "../include/relatorio.h"
#include "../include/metricas.h"
#include "../include/jogo.h"
#include "../include/config.h"

#define TAM_ENTRADA   256
#define HIST_MOSTRAR  20

#define LARGURA_TELA  1280
#define ALTURA_TELA   720

// mesmas cores do jogo
#define COR_FUNDO     (Color){ 245, 238, 226, 255 }
#define COR_PAINEL    (Color){ 255, 251, 243, 255 }
#define COR_BORDA     (Color){ 222, 208, 188, 255 }
#define COR_TEXTO     (Color){  40,  36,  32, 255 }
#define COR_SUAVE     (Color){ 128, 116, 104, 255 }
#define COR_LARANJA   (Color){ 232, 128,  20, 255 }
#define COR_LARANJA_C (Color){ 253, 232, 204, 255 }
#define COR_PAREDE    (Color){  26,  26,  26, 255 }
#define COR_MESA      (Color){  52, 204,  96, 255 }
#define COR_MESA_ESC  (Color){  30, 150,  66, 255 }
#define COR_CLIENTE   (Color){  66, 133, 244, 255 }
#define COR_CLIENTE_E (Color){  34,  88, 186, 255 }
#define COR_JOGADOR   (Color){ 246, 146,  30, 255 }
#define COR_JOGADOR_E (Color){ 180,  92,   8, 255 }

typedef struct {
    Rectangle   r;
    const char *tecla;   // o que seria digitado no terminal
    const char *texto;
} Botao;

static void texto_centro(const char *t, float cx, float y, int tam, Color c){
    DrawText(t, (int) (cx - (float) MeasureText(t, tam) / 2.0f), (int) y, tam, c);
}

static void painel(Rectangle r, float raio, Color fundo, Color borda){
    float menor = (r.width < r.height) ? r.width : r.height;
    DrawRectangleRounded(r, 2.0f * raio / menor, 8, borda);
    Rectangle dentro = { r.x + 2, r.y + 2, r.width - 4, r.height - 4 };
    DrawRectangleRounded(dentro, 2.0f * (raio - 2) / (menor - 4), 8, fundo);
}

static void desenhar_botao(const Botao *b){
    int em_cima = CheckCollisionPointRec(GetMousePosition(), b->r);
    painel(b->r, 10, em_cima ? COR_LARANJA_C : COR_PAINEL, em_cima ? COR_LARANJA : COR_BORDA);
    Rectangle chip = { b->r.x + 12, b->r.y + (b->r.height - 32) / 2, 32, 32 };
    DrawRectangleRounded(chip, 0.3f, 6, COR_LARANJA);
    texto_centro(b->tecla, chip.x + 16, chip.y + 6, 20, RAYWHITE);
    DrawText(b->texto, (int) (chip.x + 50), (int) (b->r.y + (b->r.height - 20) / 2), 20, COR_TEXTO);
}

static void desenhar_caixa_texto(Rectangle r, const char *buf){
    painel(r, 8, WHITE, COR_LARANJA);
    const char *p = buf;
    while (*p != '\0' && (float) MeasureText(p, 20) > r.width - 50){
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

// quebra o texto em linhas que cabem na largura, devolve quantas
static int quebrar_texto(const char *texto, int tam, float largura, char linhas[][200], int max){
    int n = 0;
    char atual[200] = "";
    const char *p = texto;
    while (*p != '\0' && n < max){
        char palavra[200];
        int k = 0;
        while (*p == ' '){
            p++;
        }
        while (*p != '\0' && *p != ' ' && k < 199){
            palavra[k++] = *p++;
        }
        palavra[k] = '\0';
        if (k == 0){
            break;
        }
        char teste[400];
        if (atual[0] != '\0'){
            snprintf(teste, sizeof teste, "%s %s", atual, palavra);
        }else{
            snprintf(teste, sizeof teste, "%s", palavra);
        }
        if (atual[0] != '\0' && (float) MeasureText(teste, tam) > largura){
            snprintf(linhas[n++], 200, "%s", atual);
            snprintf(atual, sizeof atual, "%s", palavra);
        }else{
            snprintf(atual, sizeof atual, "%.199s", teste);
        }
    }
    if (atual[0] != '\0' && n < max){
        snprintf(linhas[n++], 200, "%s", atual);
    }
    return n;
}

// paragrafo com quebra de linha, devolve o y de baixo
static float paragrafo(const char *texto, float x, float y, float largura, Color cor){
    char linhas[8][200];
    int n = quebrar_texto(texto, 20, largura, linhas, 8);
    for (int i = 0; i < n; i++){
        DrawText(linhas[i], (int) x, (int) y, 20, cor);
        y += 24;
    }
    return y;
}

static double g_t_apagar = 0.0;

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

static int ler_linha(char *buf, int tam, void (*desenhar)(const char *buf, const Metricas *metricas), const Metricas *metricas, const Botao *botoes, int nb){
    buf[0] = '\0';
    while (!WindowShouldClose()){
        BeginDrawing();
        desenhar(buf, metricas);
        EndDrawing();
        // le depois de desenhar pra nao pegar o enter/clique da tela anterior
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            for (int i = 0; i < nb; i++){
                if (CheckCollisionPointRec(GetMousePosition(), botoes[i].r)){
                    snprintf(buf, (size_t) tam, "%s", botoes[i].tecla);
                    return 1;
                }
            }
        }
        if (teclado_ler(buf, tam)){
            return 1;
        }
    }
    return 0;
}

static void esperar_enter(void (*desenhar)(void)){
    while (!WindowShouldClose()){
        BeginDrawing();
        desenhar();
        EndDrawing();
        if (apertou_enter_ou_esc() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            return;
        }
    }
}

// enfeite com as formas do mapa (cliente, jogador, mesa)
static void desenhar_cliente(float cx, float cy, float tam){
    DrawRectangleRec((Rectangle){ cx - tam / 2, cy - tam / 2, tam, tam }, COR_CLIENTE_E);
    DrawRectangleRec((Rectangle){ cx - tam / 2 + 3, cy - tam / 2 + 3, tam - 6, tam - 6 }, COR_CLIENTE);
}

static void desenhar_jogador(float cx, float cy, float tam){
    DrawTriangle((Vector2){ cx, cy - tam - 3 }, (Vector2){ cx - tam - 3, cy + tam * 0.75f + 2 },
                 (Vector2){ cx + tam + 3, cy + tam * 0.75f + 2 }, COR_JOGADOR_E);
    DrawTriangle((Vector2){ cx, cy - tam }, (Vector2){ cx - tam, cy + tam * 0.75f },
                 (Vector2){ cx + tam, cy + tam * 0.75f }, COR_JOGADOR);
}

static void desenhar_mesa(float x, float y){
    DrawRectangle((int) x, (int) y, 80, 70, COR_MESA_ESC);
    DrawRectangle((int) x + 4, (int) y + 4, 72, 62, COR_MESA);
    DrawRectangle((int) x - 30, (int) y + 24, 22, 22, COR_MESA_ESC);
    DrawRectangle((int) x - 27, (int) y + 27, 16, 16, COR_MESA);
    DrawRectangle((int) x + 88, (int) y + 24, 22, 22, COR_MESA_ESC);
    DrawRectangle((int) x + 91, (int) y + 27, 16, 16, COR_MESA);
}

static void desenhar_enfeite(void){
    // fila de clientes e balcao a esquerda, mesas a direita
    DrawRectangleRounded((Rectangle){ 60, 560, 120, 14 }, 1.0f, 6, COR_PAREDE);
    DrawRectangleRounded((Rectangle){ 250, 560, 120, 14 }, 1.0f, 6, COR_PAREDE);
    for (int i = 0; i < 6; i++){
        desenhar_cliente(215, 530.0f - (float) i * 46.0f, 34);
    }
    desenhar_jogador(215, 625, 18);
    desenhar_mesa(1010, 250);
    desenhar_mesa(1060, 420);
    desenhar_cliente(991, 285, 28);
    desenhar_cliente(1159, 455, 28);
}


static void tela_carregando(void){
    // dois frames pra garantir que aparece antes da carga pesada
    for (int i = 0; i < 2; i++){
        BeginDrawing();
        ClearBackground(COR_FUNDO);
        texto_centro("DADO BURGER", LARGURA_TELA / 2.0f, 280, 60, COR_LARANJA);
        texto_centro("Carregando pedidos e histórico...", LARGURA_TELA / 2.0f, 370, 20, COR_SUAVE);
        EndDrawing();
    }
}

static const Botao BOTOES_MENU[4] = {
    { { 440, 236, 400, 54 }, "1", "Novo jogo"  },
    { { 440, 300, 400, 54 }, "2", "Como jogar" },
    { { 440, 364, 400, 54 }, "3", "Histórico"  },
    { { 440, 428, 400, 54 }, "0", "Sair"       },
};

static void desenhar_menu(const char *buf, const Metricas *metricas){
    ClearBackground(COR_FUNDO);
    desenhar_enfeite();
    texto_centro("DADO BURGER", LARGURA_TELA / 2.0f + 4, 84, 60, COR_TEXTO);
    texto_centro("DADO BURGER", LARGURA_TELA / 2.0f, 80, 60, COR_LARANJA);
    texto_centro("Monte o hambúrguer exatamente como o pedido!", LARGURA_TELA / 2.0f, 162, 20, COR_SUAVE);
    for (int i = 0; i < 4; i++){
        desenhar_botao(&BOTOES_MENU[i]);
    }
    desenhar_caixa_texto((Rectangle){ 440, 512, 400, 52 }, buf);
    texto_centro("Digite a opção e aperte Enter, ou clique", LARGURA_TELA / 2.0f, 578, 20, COR_SUAVE);
    texto_centro(TextFormat("%lld pedidos carregados  |  %d entradas no histórico",
                            metricas->avl.nos_inseridos, metricas->historico.entradas_carregadas),
                 LARGURA_TELA / 2.0f, 680, 10, COR_SUAVE);
}

static float chip(float x, float y, Color cor, const char *texto){
    DrawRectangle((int) x, (int) y + 2, 16, 16, (Color){ (unsigned char) (cor.r * 0.7f), (unsigned char) (cor.g * 0.7f), (unsigned char) (cor.b * 0.7f), 255 });
    DrawRectangle((int) x + 2, (int) y + 4, 12, 12, cor);
    DrawText(texto, (int) x + 22, (int) y, 20, COR_TEXTO);
    return x + 22 + (float) MeasureText(texto, 20) + 26;
}

// tela estatica com as regras
static void desenhar_como_jogar(void){
    ClearBackground(COR_FUNDO);
    painel((Rectangle){ 40, 16, 1200, 688 }, 14, COR_PAINEL, COR_BORDA);
    texto_centro("COMO JOGAR", LARGURA_TELA / 2.0f, 32, 40, COR_LARANJA);

    float x = 80, w = 1120, y = 90;
    DrawText("Objetivo", (int) x, (int) y, 20, COR_LARANJA);
    y = paragrafo("Atender todos os clientes do Dado Burger montando o hambúrguer exatamente como o pedido, "
                  "antes que a paciência do cliente da frente acabe.", x, y + 26, w, COR_TEXTO) + 10;

    DrawText("Ingredientes (símbolo)", (int) x, (int) y, 20, COR_LARANJA);
    y += 28;
    DrawText("Sem preparo, infinitos:", (int) x, (int) y, 20, COR_SUAVE);
    float cx = x + 300;
    cx = chip(cx, y, (Color){ 224, 155, 70, 255 }, "pão de cima (z)");
    cx = chip(cx, y, (Color){ 214, 52, 44, 255 }, "ketchup (k)");
    cx = chip(cx, y, (Color){ 240, 196, 40, 255 }, "mostarda (m)");
    cx = chip(cx, y, (Color){ 98, 158, 58, 255 }, "picles (p)");
    chip(cx, y, (Color){ 122, 206, 82, 255 }, "alface (a)");
    y += 28;
    DrawText("Com preparo, duram:", (int) x, (int) y, 20, COR_SUAVE);
    cx = x + 300;
    cx = chip(cx, y, (Color){ 246, 224, 128, 255 }, "queijo suíço (s)");
    cx = chip(cx, y, (Color){ 246, 166, 34, 255 }, "cheddar (q)");
    cx = chip(cx, y, (Color){ 214, 170, 222, 255 }, "cebola (c)");
    chip(cx, y, (Color){ 216, 58, 52, 255 }, "tomate (t)");
    y += 28;
    DrawText("Com preparo, perecem:", (int) x, (int) y, 20, COR_SUAVE);
    cx = x + 300;
    cx = chip(cx, y, (Color){ 112, 62, 32, 255 }, "carne (n)");
    cx = chip(cx, y, (Color){ 250, 250, 244, 255 }, "ovo frito (o)");
    chip(cx, y, (Color){ 204, 64, 58, 255 }, "bacon (b)");
    y += 40;

    DrawText("Ações (cada uma consome 1 turno)", (int) x, (int) y, 20, COR_LARANJA);
    y += 28;
    static const char *acoes[4][2] = {
        { "p <símbolo>", "preparar um ingrediente na chapa (ex: p n)" },
        { "a <símbolo>", "adicionar um ingrediente ao hambúrguer (ex: a k)" },
        { "r",           "remover o ingrediente do topo (ele vai pra lixeira)" },
        { "s",           "sair da partida (não consome turno)" },
    };
    for (int i = 0; i < 4; i++){
        DrawText(acoes[i][0], (int) x + 20, (int) y, 20, COR_TEXTO);
        DrawText(acoes[i][1], (int) x + 200, (int) y, 20, COR_SUAVE);
        y += 26;
    }
    y += 12;

    DrawText("Regras", (int) x, (int) y, 20, COR_LARANJA);
    y += 28;
    y = paragrafo("O pão de baixo já está no prato. Feche o hambúrguer com o pão de cima (z).", x, y, w, COR_TEXTO);
    y = paragrafo("Perecíveis vencem após alguns turnos e são descartados do estoque.", x, y, w, COR_TEXTO);
    y = paragrafo("Vitória: todos os clientes atendidos. Derrota: a paciência do cliente da frente chega a zero.",
                  x, y, w, COR_TEXTO);
    y += 12;

    DrawText("No mapa", (int) x, (int) y, 20, COR_LARANJA);
    y += 28;
    cx = x;
    cx = chip(cx, y, COR_PAREDE, "parede");
    cx = chip(cx, y, COR_MESA, "mesas");
    cx = chip(cx, y, COR_CLIENTE, "clientes");
    cx = chip(cx, y, COR_JOGADOR, "você");
    cx = chip(cx, y, (Color){ 250, 204, 21, 255 }, "montagem");
    cx = chip(cx, y, (Color){ 182, 76, 204, 255 }, "entrega");
    cx = chip(cx, y, (Color){ 228, 62, 50, 255 }, "chapa");
    chip(cx, y, (Color){ 110, 110, 110, 255 }, "lixeira");

    texto_centro("Pressione Enter para voltar ao menu...", LARGURA_TELA / 2.0f, 668, 20, COR_TEXTO);
}

static void tela_como_jogar(void){
    esperar_enter(desenhar_como_jogar);
}

static const Botao BOTOES_HIST[2] = {
    { { 420, 290, 440, 54 }, "1", "Ordem alfabética"      },
    { { 420, 354, 440, 54 }, "2", "Pontuação decrescente" },
};

static void desenhar_escolha_historico(const char *buf, const Metricas *metricas){
    ClearBackground(COR_FUNDO);
    painel((Rectangle){ 340, 140, 600, 440 }, 14, COR_PAINEL, COR_BORDA);
    texto_centro("HISTÓRICO", LARGURA_TELA / 2.0f, 170, 40, COR_LARANJA);
    texto_centro("Como você quer ordenar?", LARGURA_TELA / 2.0f, 236, 20, COR_SUAVE);
    for (int i = 0; i < 2; i++){
        desenhar_botao(&BOTOES_HIST[i]);
    }
    desenhar_caixa_texto((Rectangle){ 420, 440, 440, 52 }, buf);
    texto_centro("Digite a opção e aperte Enter, ou clique", LARGURA_TELA / 2.0f, 512, 20, COR_SUAVE);
}

// o que a tela da lista precisa (preenchido antes de desenhar)
static struct {
    const EntradaHistorico *copia;
    int    n;
    int    mostrados;
    double tempo;
    int    tipo;
} g_hist;

static void desenhar_lista_historico(void){
    ClearBackground(COR_FUNDO);
    painel((Rectangle){ 140, 24, 1000, 672 }, 14, COR_PAINEL, COR_BORDA);
    texto_centro("HISTÓRICO", LARGURA_TELA / 2.0f, 44, 40, COR_LARANJA);
    texto_centro(g_hist.tipo == ORDENAR_POR_PONTUACAO ? "Pontuação decrescente" : "Ordem alfabética",
                 LARGURA_TELA / 2.0f, 92, 20, COR_SUAVE);

    // duas colunas de 10
    for (int c = 0; c < 2; c++){
        float x = (c == 0) ? 190 : 670;
        DrawText("#", (int) x, 132, 20, COR_SUAVE);
        DrawText("Nome", (int) x + 56, 132, 20, COR_SUAVE);
        const char *pts = "Pontos";
        DrawText(pts, (int) (x + 420 - MeasureText(pts, 20)), 132, 20, COR_SUAVE);
        DrawRectangle((int) x, 156, 420, 2, COR_BORDA);
    }
    for (int i = 0; i < g_hist.mostrados; i++){
        int c = (i < HIST_MOSTRAR / 2) ? 0 : 1;
        int l = i % (HIST_MOSTRAR / 2);
        float x = (c == 0) ? 190 : 670;
        float y = 168.0f + (float) l * 42.0f;
        if (l % 2 == 0){
            DrawRectangle((int) x - 8, (int) y - 8, 436, 40, (Color){ 248, 240, 226, 255 });
        }
        const char *pos = TextFormat("%d.", i + 1);
        DrawText(pos, (int) (x + 36 - MeasureText(pos, 20)), (int) y + 4, 20, COR_SUAVE);
        DrawText(g_hist.copia[i].nome, (int) x + 56, (int) y + 4, 20, COR_TEXTO);
        const char *p = TextFormat("%d", g_hist.copia[i].pontuacao);
        DrawText(p, (int) (x + 420 - MeasureText(p, 20)), (int) y + 4, 20, COR_LARANJA);
    }

    texto_centro(TextFormat("(%d de %d entradas)   %s: %.6f s", g_hist.mostrados, g_hist.n,
                            nome_algoritmo_ordenacao(), g_hist.tempo),
                 LARGURA_TELA / 2.0f, 610, 20, COR_SUAVE);
    texto_centro("Pressione Enter para voltar ao menu", LARGURA_TELA / 2.0f, 650, 20, COR_TEXTO);
}

// ordena uma copia do historico e mostra so os primeiros
static void tela_historico(const Historico *historico, Metricas *metricas){
    char linha[TAM_ENTRADA];
    int escolha = 0;
    if (!ler_linha(linha, TAM_ENTRADA, desenhar_escolha_historico, metricas, BOTOES_HIST, 2)){
        return;
    }
    sscanf(linha, "%d", &escolha);

    CriterioOrdenacao criterio;
    criterio.tipo = ORDENAR_POR_NOME;
    if (escolha == 2){
        criterio.tipo = ORDENAR_POR_PONTUACAO;
    }

    EntradaHistorico *copia;
    int n;
    double tempo;
    ordenar_historico(criterio, historico, metricas, &copia, &n, &tempo);

    // n pode ser maior que o limite da tela
    int mostrados = n;
    if (n > HIST_MOSTRAR){
        mostrados = HIST_MOSTRAR;
    }

    g_hist.copia = copia;
    g_hist.n = n;
    g_hist.mostrados = mostrados;
    g_hist.tempo = tempo;
    g_hist.tipo = criterio.tipo;
    esperar_enter(desenhar_lista_historico);

    liberar_copia_ordenada(copia);
}

// carrega pedidos e historico dos argumentos e abre o menu
int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc < 3){
        fprintf(stderr, "Uso: %s <arquivo_pedidos> <arquivo_historico>\n",
                argv[0]);
        return 1;
    }

    const char *arq_pedidos   = argv[1];
    const char *arq_historico = argv[2];

    // tudo que o jogo usa fica aqui e e passado por parametro
    Metricas    metricas;
    ArvoreAVL   avl;
    ArvoreSplay splay;
    TabelaHash  hash;
    Historico   historico;
    memset(&metricas, 0, sizeof metricas);
    memset(&historico, 0, sizeof historico);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Dado Burger");
    if (!IsWindowReady()){
        fprintf(stderr, "Erro: nao foi possivel abrir a janela\n");
        return 1;
    }
    SetExitKey(KEY_NULL);   // esc so limpa o texto, quem fecha e o X da janela
    SetTargetFPS(60);
    tela_carregando();

    if (!carregar_pedidos(arq_pedidos, &avl, &splay, &metricas)){
        fprintf(stderr, "Erro: nao foi possivel carregar '%s'\n",
                arq_pedidos);
        CloseWindow();
        return 1;
    }

    carregar_historico(arq_historico, &historico, &metricas);

    // a hash comeca vazia, e recriada a cada partida
    hash_inicializar(&hash, CFG_HASH_M);
    metricas.hash.m = hash.m;

    // grava no log as metricas do carregamento
    registrar_log_carregamento(CFG_ARQUIVO_LOG, &metricas);

    char linha[TAM_ENTRADA];
    int opcao = -1;
    while (opcao != 0){
        if (!ler_linha(linha, TAM_ENTRADA, desenhar_menu, &metricas, BOTOES_MENU, 4)){
            break;
        }
        opcao = -1;
        sscanf(linha, "%d", &opcao);

        if (opcao == 1){
            jogo_executar(&avl, &splay, &hash, &historico, &metricas);
        }else if (opcao == 2){
            tela_como_jogar();
        }else if (opcao == 3){
            tela_historico(&historico, &metricas);
        }
    }

    liberar_historico(&historico);
    hash_destruir(&hash);
    liberar_pedidos(&avl, &splay);
    CloseWindow();
    return 0;
}