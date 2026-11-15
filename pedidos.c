#include "pedidos.h"
#include "avl.h"
#include "splay.h"
#include "hash.h"
#include "metricas.h"
#include "timer.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CAPACIDADE_INICIAL 4096
#define TAM_LINHA          256

static Pedido     *g_todos      = NULL;
static int         g_total      = 0;
static int         g_capacidade = 0;

static ArvoreAVL   g_avl;
static ArvoreSplay g_splay;
static TabelaHash  g_cache;
static int         g_carregado = 0;
static int         g_splay_ativa = 0;

static unsigned int g_aleatorio = 2463534242u;

static unsigned int proximo_aleatorio(void) {
    unsigned int x = g_aleatorio;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_aleatorio = x;
    return x;
}

void pedidos_definir_semente(unsigned int semente) {
    g_aleatorio = semente;
}

static int ler_linha(FILE *f, char *buf, int tam) {
    if (fgets(buf, tam, f) == NULL)
        return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

static void guardar_no_vetor(int codigo, const char *receita) {
    if (g_total == g_capacidade) {
        g_capacidade = g_capacidade * 2;
        g_todos = realloc(g_todos, g_capacidade * sizeof(Pedido));
    }
    g_todos[g_total].codigo = codigo;
    strcpy(g_todos[g_total].receita, receita);
    g_total++;
}

int carregar_pedidos(const char *caminho_arquivo) {
    liberar_pedidos();

    memset(&g_metricas.avl, 0, sizeof(MetricasArvore));
    memset(&g_metricas.splay, 0, sizeof(MetricasArvore));
    memset(&g_metricas.carga, 0, sizeof(MetricasCarga));
    memset(&g_metricas.hash, 0, sizeof(MetricasHash));
    g_metricas.splay_ativa = 0;

    pedidos_definir_semente((unsigned int) time(NULL) ^ ((unsigned int) clock() * 2654435761u));

    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL)
        return 0;

    g_capacidade = CAPACIDADE_INICIAL;
    g_todos = malloc(g_capacidade * sizeof(Pedido));

    Timer t_total;
    timer_iniciar(&t_total);

    Timer t_leitura;
    timer_iniciar(&t_leitura);

    char linha_codigo[TAM_LINHA];
    char linha_receita[TAM_LINHA];

    while (ler_linha(f, linha_codigo, TAM_LINHA) && ler_linha(f, linha_receita, TAM_LINHA))
        guardar_no_vetor(atoi(linha_codigo), linha_receita);
    fclose(f);
    g_metricas.carga.tempo_leitura_s = timer_parar(&t_leitura);

    avl_inicializar(&g_avl, &g_metricas.avl);

    Timer t_avl;
    timer_iniciar(&t_avl);
    int guardados = 0;
    for (int i = 0; i < g_total; i++) {
        if (avl_inserir(&g_avl, &g_todos[i]) == 1)
            g_todos[guardados++] = g_todos[i];
    }
    g_metricas.avl.tempo_insercao_s = timer_parar(&t_avl);
    g_total = guardados;
    g_metricas.avl.tempo_carga_s = timer_parar(&t_total);
    g_metricas.avl.altura = avl_altura(&g_avl);

    g_splay_ativa = 0;
    splay_inicializar(&g_splay, &g_metricas.splay);
    if (CFG_COMPARAR_SPLAY) {
        Timer t_splay;
        timer_iniciar(&t_splay);
        for (int i = 0; i < g_total; i++) {
            splay_inserir(&g_splay, &g_todos[i]);
        }
        g_metricas.splay.tempo_insercao_s = timer_parar(&t_splay);

        splay_calcular_altura(&g_splay);
        g_metricas.splay.tempo_carga_s = g_metricas.carga.tempo_leitura_s + g_metricas.splay.tempo_insercao_s;
        g_splay_ativa = 1;
        g_metricas.splay_ativa = 1;
    }

    hash_inicializar(&g_cache, CFG_HASH_M, &g_metricas.hash);

    g_carregado = 1;
    return 1;
}

int get_pedido(int codigo, Pedido *pedido_out) {
    if (hash_buscar(&g_cache, codigo, pedido_out))
        return 1;

    const Pedido *achou = avl_buscar(&g_avl, codigo);

    if (g_splay_ativa)
        splay_buscar(&g_splay, codigo);

    if (achou == NULL)
        return 0;

    *pedido_out = *achou;
    hash_inserir(&g_cache, achou);
    return 1;
}

int sortear_codigo_pedido(void) {
    unsigned int indice = proximo_aleatorio() % (unsigned int) g_total;
    return g_todos[indice].codigo;
}

void pedidos_iniciar_partida(void) {
    g_metricas.avl.buscas = 0;
    g_metricas.avl.comparacoes = 0;
    g_metricas.avl.tempo_busca_s = 0.0;
    g_metricas.avl.rotacoes_busca = 0;

    g_metricas.splay.buscas = 0;
    g_metricas.splay.comparacoes = 0;
    g_metricas.splay.tempo_busca_s = 0.0;
    g_metricas.splay.rotacoes_busca = 0;
}

void receita_para_lista(const char *receita, Lista *lista_out) {
    lista_inicializar(lista_out);
    for (int i = 0; receita[i] != '\0'; i++) {
        inserir_fim(lista_out, receita[i]);
    }
}

void liberar_pedidos(void) {
    hash_destruir(&g_cache);
    splay_destruir(&g_splay);
    avl_destruir(&g_avl);
    free(g_todos);
    g_todos = NULL;
    g_total = 0;
    g_capacidade = 0;
    g_carregado = 0;
    g_splay_ativa = 0;
}