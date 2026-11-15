#include "ordenacao.h"
#include "historico.h"
#include "metricas.h"
#include "relatorio.h"
#include "timer.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>


static int minuscula(unsigned char c) {
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');
    return c;
}

static int comparar_nome(const EntradaHistorico *a, const EntradaHistorico *b) {
    const unsigned char *x = (const unsigned char*) a->nome;
    const unsigned char *y = (const unsigned char*) b->nome;
    while (*x != '\0' && *y != '\0') {
        int cx = minuscula(*x);
        int cy = minuscula(*y);
        if (cx != cy)
            return cx - cy;
        x++;
        y++;
    }
    return minuscula(*x) - minuscula(*y);
}

static int comparar_pontuacao(const EntradaHistorico *a, const EntradaHistorico *b) {
    if (a->pontuacao > b->pontuacao) return -1;
    if (a->pontuacao < b->pontuacao) return 1;
    return 0;
}

typedef int (*FuncaoComparar)(const EntradaHistorico*, const EntradaHistorico*);

static void intercalar(EntradaHistorico *v, EntradaHistorico *aux,
                       int ini, int meio, int fim, FuncaoComparar cmp) {
    memcpy(aux + ini, v + ini, (size_t) (fim - ini + 1) * sizeof(EntradaHistorico));

    int i = ini;
    int j = meio + 1;
    int k = ini;

    while (i <= meio && j <= fim) {
        if (cmp(&aux[i], &aux[j]) <= 0)
            v[k++] = aux[i++];
        else
            v[k++] = aux[j++];
    }
    while (i <= meio) {
        v[k++] = aux[i++];
    }
    while (j <= fim) {
        v[k++] = aux[j++];
    }
}

static void merge_sort_rec(EntradaHistorico *v, EntradaHistorico *aux,
                           int ini, int fim, FuncaoComparar cmp) {
    if (ini >= fim)
        return;
    int meio = ini + (fim - ini) / 2;
    merge_sort_rec(v, aux, ini, meio, cmp);
    merge_sort_rec(v, aux, meio + 1, fim, cmp);
    intercalar(v, aux, ini, meio, fim, cmp);
}

static void merge_sort(EntradaHistorico *v, int n, FuncaoComparar cmp) {
    EntradaHistorico *aux = malloc((size_t) n * sizeof(EntradaHistorico));
    merge_sort_rec(v, aux, 0, n - 1, cmp);
    free(aux);
}

void ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out) {
    int n;
    const EntradaHistorico *original = obter_entradas_historico(&n);

    EntradaHistorico *copia = malloc((size_t) n * sizeof(EntradaHistorico));
    memcpy(copia, original, (size_t) n * sizeof(EntradaHistorico));

    FuncaoComparar cmp = comparar_pontuacao;
    if (criterio == ORDENAR_POR_NOME)
        cmp = comparar_nome;
    Timer t;
    timer_iniciar(&t);
    merge_sort(copia, n, cmp);
    double segundos = timer_parar(&t);

    MetricasOrdenacao *mo = &g_metricas.ordenacao;
    int c = (int) criterio;
    mo->medicoes[c]++;
    mo->ultimo_tempo_s[c] = segundos;
    mo->n_entradas[c] = n;
    if (mo->medicoes[c] == 1 || segundos < mo->melhor_tempo_s[c])
        mo->melhor_tempo_s[c] = segundos;

    atualizar_log_ordenacao(CFG_ARQUIVO_LOG, segundos, n);

    *copia_out = copia;
    *n_out = n;
    *tempo_out = segundos;
}

void liberar_copia_ordenada(EntradaHistorico *copia) {
    free(copia);
}

const char* nome_algoritmo_ordenacao(void) {
    return "Merge Sort";
}
