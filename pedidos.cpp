#include "pedidos.hpp"
#include "avl.hpp"
#include "splay.hpp"
#include "hash.hpp"
#include "metricas.hpp"
#include "timer.hpp"
#include "config.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define CAPACIDADE_INICIAL 4096
#define TAM_LINHA          256

// data of the module, only this file can see it
static Pedido     *g_todos      = NULL;   // all the orders, in a plain array
static int         g_total      = 0;      // orders in the array
static int         g_capacidade = 0;      // orders that fit in the array

static ArvoreAVL   g_avl;                 // main storage
static ArvoreSplay g_splay;               // extra, for the comparison
static TabelaHash  g_cache;               // cache of the orders already searched
static bool        g_carregado = false;   // true after a successful load
static bool        g_splay_ativa = false; // true if the splay tree was built

static unsigned int g_aleatorio = 2463534242u;   // state of the random generator

// small random generator (xorshift32), it needs a start value that is not zero
// we do not use rand() because on windows it only gives numbers up to 32767
static unsigned int proximo_aleatorio() {
    unsigned int x = g_aleatorio;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_aleatorio = x;
    return x;
}

void pedidos_definir_semente(unsigned int semente) {
    g_aleatorio = (semente == 0) ? 2463534242u : semente;
}

// reads the next line that is not empty and removes spaces and the end of line
// returns false at the end of the file
static bool ler_linha_nao_vazia(FILE *f, char *buf, int tam) {
    while (fgets(buf, tam, f) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n' && len == (size_t) tam - 1) {
            int c;
            while ((c = fgetc(f)) != EOF && c != '\n') {
                // throws away the rest of the long line
            }
        }
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r' ||
                           buf[len - 1] == ' '  || buf[len - 1] == '\t')) {
            buf[--len] = '\0';
        }
        if (len > 0) {
            return true;                        // found a line with text
        }
    }
    return false;
}

// reads a whole number from a text, returns false if the text is not a number
static bool ler_inteiro(const char *texto, int *valor) {
    char *fim = NULL;
    errno = 0;
    long v = strtol(texto, &fim, 10);
    if (fim == texto || *fim != '\0' || errno == ERANGE || v > 2147483647L || v < -2147483647L) {
        return false;
    }
    *valor = (int) v;
    return true;
}

// a recipe is valid if it has 1 to 7 letters and every letter is an ingredient
static bool receita_valida(const char *r) {
    size_t n = strlen(r);
    if (n == 0 || n > TAM_MAX_RECEITA) {
        return false;
    }
    for (size_t i = 0; i < n; i++) {
        if (strchr("zkmpasqctnob", r[i]) == NULL) {
            return false;
        }
    }
    return true;
}

// puts one order at the end of the array (the array grows when it is full)
static bool guardar_no_vetor(int codigo, const char *receita) {
    if (g_total == g_capacidade) {
        int nova = (g_capacidade == 0) ? CAPACIDADE_INICIAL : g_capacidade * 2;
        Pedido *novo = (Pedido*) realloc(g_todos, (size_t) nova * sizeof(Pedido));
        if (novo == NULL) {
            return false;
        }
        g_todos = novo;
        g_capacidade = nova;
    }
    g_todos[g_total].codigo = codigo;           // stores the order in the array
    strcpy(g_todos[g_total].receita, receita);
    g_total++;
    return true;
}

bool carregar_pedidos(const char *caminho_arquivo) {
    liberar_pedidos();                          // starts clean, even if called twice

    // zeros the counters of everything this module owns
    g_metricas.avl = MetricasArvore();
    g_metricas.splay = MetricasArvore();
    g_metricas.carga = MetricasCarga();
    g_metricas.hash = MetricasHash();
    g_metricas.splay_ativa = 0;

    // the random generator starts from the clock, so every run is different
    pedidos_definir_semente((unsigned int) time(NULL) ^ ((unsigned int) clock() * 2654435761u));

    FILE *f = fopen(caminho_arquivo, "r");
    if (f == NULL) {
        return false;
    }

    Timer t_total;
    timer_iniciar(&t_total);                    // whole load time: read the file and build the avl

    // step 1: reads the file into the array
    Timer t_leitura;
    timer_iniciar(&t_leitura);

    char linha_codigo[TAM_LINHA];
    char linha_receita[TAM_LINHA];
    bool primeira = true;
    bool memoria_ok = true;

    while (ler_linha_nao_vazia(f, linha_codigo, TAM_LINHA)) {   // reads the code line
        char *texto_codigo = linha_codigo;
        if (primeira) {
            primeira = false;
            // removes the invisible bom at the start of the file
            if ((unsigned char) texto_codigo[0] == 0xEF && (unsigned char) texto_codigo[1] == 0xBB &&
                (unsigned char) texto_codigo[2] == 0xBF) {
                texto_codigo += 3;
            }
        }
        if (!ler_linha_nao_vazia(f, linha_receita, TAM_LINHA)) {
            g_metricas.carga.linhas_invalidas++;   // a code without recipe at the end
            break;
        }

        int codigo = 0;
        const char *receita = linha_receita;
        while (*receita == ' ' || *receita == '\t') {
            receita++;                          // ignores spaces at the start
        }
        if (!ler_inteiro(texto_codigo, &codigo) || !receita_valida(receita)) {
            g_metricas.carga.linhas_invalidas++;   // wrong order, ignores it
            continue;
        }
        if (!guardar_no_vetor(codigo, receita)) {
            memoria_ok = false;
            break;
        }
    }
    fclose(f);
    g_metricas.carga.tempo_leitura_s = timer_parar(&t_leitura);

    if (!memoria_ok || g_total == 0) {
        liberar_pedidos();
        return false;
    }

    // step 2: builds the avl tree
    avl_inicializar(&g_avl, &g_metricas.avl);

    Timer t_avl;
    timer_iniciar(&t_avl);
    int guardados = 0;                          // orders that stay (not repeated)
    for (int i = 0; i < g_total; i++) {
        int r = avl_inserir(&g_avl, &g_todos[i]);
        if (r == 1) {
            g_todos[guardados++] = g_todos[i];  // keeps it, and packs the array without repeated codes
        } else if (r == 0) {
            g_metricas.carga.codigos_repetidos++;
        } else {
            liberar_pedidos();                  // no memory
            return false;
        }
    }
    g_metricas.avl.tempo_insercao_s = timer_parar(&t_avl);
    g_total = guardados;
    g_metricas.avl.tempo_carga_s = timer_parar(&t_total);
    g_metricas.avl.altura = avl_altura(&g_avl);

    // step 3: builds the splay tree (extra, only to compare)
    g_splay_ativa = false;
    splay_inicializar(&g_splay, &g_metricas.splay);
    if (CFG_COMPARAR_SPLAY) {
        Timer t_splay;
        timer_iniciar(&t_splay);
        bool splay_ok = true;
        for (int i = 0; i < g_total; i++) {
            if (splay_inserir(&g_splay, &g_todos[i]) != 1) {
                splay_ok = false;
                break;
            }
        }
        g_metricas.splay.tempo_insercao_s = timer_parar(&t_splay);

        if (splay_ok && splay_calcular_altura(&g_splay) >= 0) {
            // load time of the splay = the same file reading + its own insertions
            g_metricas.splay.tempo_carga_s = g_metricas.carga.tempo_leitura_s + g_metricas.splay.tempo_insercao_s;
            g_splay_ativa = true;
            g_metricas.splay_ativa = 1;
        } else {
            splay_destruir(&g_splay);           // the extra failed, we go on without it
            g_metricas.splay = MetricasArvore();
        }
    }

    // step 4: starts the cache
    if (!hash_inicializar(&g_cache, CFG_HASH_M, &g_metricas.hash)) {
        liberar_pedidos();
        return false;
    }

    g_carregado = true;
    return true;
}

bool get_pedido(int codigo, Pedido *pedido_out) {
    if (!g_carregado) {
        return false;
    }

    // step 1: asks the cache first, a hit means we do not touch the avl
    if (hash_buscar(&g_cache, codigo, pedido_out)) {
        return true;
    }

    // step 2: cache miss, searches the avl (it counts comparisons and time by itself)
    const Pedido *achou = avl_buscar(&g_avl, codigo);

    // the same search is made in the splay tree only to compare, its answer is not used
    if (g_splay_ativa) {
        splay_buscar(&g_splay, codigo);
    }

    if (achou == NULL) {
        return false;                           // this code does not exist
    }

    // step 3: saves the order in the cache for next time
    *pedido_out = *achou;
    hash_inserir(&g_cache, achou);
    return true;
}

int sortear_codigo_pedido() {
    if (!g_carregado || g_total == 0) {
        return -1;
    }
    unsigned int indice = proximo_aleatorio() % (unsigned int) g_total;   // random position in the array
    return g_todos[indice].codigo;
}

void pedidos_iniciar_partida() {
    // only the counters of the searches are cleared, the load numbers stay
    g_metricas.avl.buscas = 0;
    g_metricas.avl.comparacoes = 0;
    g_metricas.avl.tempo_busca_s = 0.0;
    g_metricas.avl.rotacoes_busca = 0;

    g_metricas.splay.buscas = 0;
    g_metricas.splay.comparacoes = 0;
    g_metricas.splay.tempo_busca_s = 0.0;
    g_metricas.splay.rotacoes_busca = 0;
}

int total_pedidos() {
    return g_total;
}

bool receita_para_lista(const char *receita, Lista *lista_out) {
    lista_inicializar(lista_out);
    for (int i = 0; receita[i] != '\0'; i++) {
        if (!inserir_fim(lista_out, receita[i])) {   // one list node for each letter
            lista_destruir(lista_out);
            lista_inicializar(lista_out);
            return false;
        }
    }
    return true;
}

void liberar_pedidos() {
    hash_destruir(&g_cache);
    splay_destruir(&g_splay);
    avl_destruir(&g_avl);
    free(g_todos);
    g_todos = NULL;
    g_total = 0;
    g_capacidade = 0;
    g_carregado = false;
    g_splay_ativa = false;
}
