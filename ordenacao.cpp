#include "ordenacao.hpp"
#include "historico.hpp"
#include "metricas.hpp"
#include "relatorio.hpp"
#include "timer.hpp"
#include "config.hpp"
#include <stdlib.h>
#include <string.h>

// the compare functions return a negative number if a goes before b
// zero if they are equal, and a positive number if a goes after b

// turns an upper case letter into lower case, other characters stay the same
static int minuscula(unsigned char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

// alphabetical order, ignoring upper and lower case
static int comparar_nome(const EntradaHistorico *a, const EntradaHistorico *b) {
    const unsigned char *x = (const unsigned char*) a->nome;
    const unsigned char *y = (const unsigned char*) b->nome;
    while (*x != '\0' && *y != '\0') {
        int cx = minuscula(*x);
        int cy = minuscula(*y);
        if (cx != cy) {
            return cx - cy;                     // first different letter decides
        }
        x++;
        y++;
    }
    return minuscula(*x) - minuscula(*y);       // the shorter name goes first
}

// biggest score first (uses < and > so there is no overflow)
static int comparar_pontuacao(const EntradaHistorico *a, const EntradaHistorico *b) {
    if (a->pontuacao > b->pontuacao) return -1;
    if (a->pontuacao < b->pontuacao) return 1;
    return 0;
}

typedef int (*FuncaoComparar)(const EntradaHistorico*, const EntradaHistorico*);

// joins two sorted parts of the array: v[ini..meio] and v[meio+1..fim]
// aux is a helper array with the same size as v
// we copy the part to aux, then take the smaller of the two halves and write it back in v
static void intercalar(EntradaHistorico *v, EntradaHistorico *aux,
                       int ini, int meio, int fim, FuncaoComparar cmp) {
    memcpy(aux + ini, v + ini, (size_t) (fim - ini + 1) * sizeof(EntradaHistorico));

    int i = ini;            // position in the left half
    int j = meio + 1;       // position in the right half
    int k = ini;            // where to write in v

    while (i <= meio && j <= fim) {
        // <= 0 (and not < 0) keeps the sort stable, the left one (older) goes first when equal
        if (cmp(&aux[i], &aux[j]) <= 0) {
            v[k++] = aux[i++];
        } else {
            v[k++] = aux[j++];
        }
    }
    while (i <= meio) {     // what is left in the left half
        v[k++] = aux[i++];
    }
    while (j <= fim) {      // what is left in the right half
        v[k++] = aux[j++];
    }
}

// sorts v[ini..fim]: cuts in two halves, sorts each half, joins them
static void merge_sort_rec(EntradaHistorico *v, EntradaHistorico *aux,
                           int ini, int fim, FuncaoComparar cmp) {
    if (ini >= fim) {
        return;             // 0 or 1 element is already sorted
    }
    int meio = ini + (fim - ini) / 2;
    merge_sort_rec(v, aux, ini, meio, cmp);
    merge_sort_rec(v, aux, meio + 1, fim, cmp);
    intercalar(v, aux, ini, meio, fim, cmp);
}

// sorts the whole array, returns false if there is no memory for the helper array
static bool merge_sort(EntradaHistorico *v, int n, FuncaoComparar cmp) {
    if (n <= 1) {
        return true;
    }
    EntradaHistorico *aux = (EntradaHistorico*) malloc((size_t) n * sizeof(EntradaHistorico));
    if (aux == NULL) {
        return false;
    }
    merge_sort_rec(v, aux, 0, n - 1, cmp);
    free(aux);
    return true;
}

bool ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out) {
    *copia_out = NULL;
    *n_out = 0;
    if (tempo_out != NULL) {
        *tempo_out = 0.0;
    }

    int n = 0;
    const EntradaHistorico *original = obter_entradas_historico(&n);

    // step 1: makes an independent copy (this part is not timed)
    EntradaHistorico *copia = NULL;
    if (n > 0) {
        copia = (EntradaHistorico*) malloc((size_t) n * sizeof(EntradaHistorico));
        if (copia == NULL) {
            return false;
        }
        memcpy(copia, original, (size_t) n * sizeof(EntradaHistorico));
    }

    // step 2: sorts the copy and measures only this part
    FuncaoComparar cmp = (criterio == ORDENAR_POR_NOME) ? comparar_nome : comparar_pontuacao;
    Timer t;
    timer_iniciar(&t);
    bool ok = merge_sort(copia, n, cmp);
    double segundos = timer_parar(&t);

    if (!ok) {
        free(copia);
        return false;
    }

    // step 3: saves the measurement (index 0 = by name, index 1 = by score)
    MetricasOrdenacao *mo = &g_metricas.ordenacao;
    int c = (int) criterio;
    mo->medicoes[c]++;
    mo->ultimo_tempo_s[c] = segundos;
    mo->n_entradas[c] = n;
    if (mo->medicoes[c] == 1 || segundos < mo->melhor_tempo_s[c]) {
        mo->melhor_tempo_s[c] = segundos;       // new fastest time
    }

    // step 4: rewrites medicoes.log with the new numbers
    atualizar_log_ordenacao(CFG_ARQUIVO_LOG, segundos, n);

    *copia_out = copia;
    *n_out = n;
    if (tempo_out != NULL) {
        *tempo_out = segundos;
    }
    return true;
}

void liberar_copia_ordenada(EntradaHistorico *copia) {
    free(copia);
}

const char* nome_algoritmo_ordenacao() {
    return "Merge Sort";
}
