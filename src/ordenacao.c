#include "../include/ordenacao.h"
#include "../include/relatorio.h"
#include "../include/timer.h"
#include "../include/config.h"
#include <stdlib.h>
#include <string.h>

// converts uppercase letter to lowercase
static char minuscula(char c){
    if (c >= 'A' && c <= 'Z'){
        return c + ('a' - 'A');
    }
    return c;
}

// compares two names ignoring letter case
static int comparar_nome(const EntradaHistorico *a, const EntradaHistorico *b){
    const char *x = a->nome;
    const char *y = b->nome;
    // checks each character until a string ends
    while (*x != '\0' && *y != '\0'){
        char cx = minuscula(*x);
        char cy = minuscula(*y);
        if (cx != cy){
            return cx - cy;
        }
        x++;
        y++;
    }
    return minuscula(*x) - minuscula(*y);
}

// compares scores from highest to lowest
static int comparar_pontuacao(const EntradaHistorico *a, const EntradaHistorico *b){
    if (a->pontuacao > b->pontuacao){
        return -1;
    }
    if (a->pontuacao < b->pontuacao){
        return 1;
    }
    return 0;
}

// defines function type for comparing entries
typedef int (*FuncaoComparar)(const EntradaHistorico*, const EntradaHistorico*);

// merges two sorted halves into one
static void intercalar(EntradaHistorico *v, EntradaHistorico *aux,
                       int ini, int meio, int fim, FuncaoComparar cmp){
    // copies range into helper array
    for (int t = ini; t <= fim; t++){
        aux[t] = v[t];
    }

    int i = ini;
    int j = meio + 1;
    int k = ini;

    // picks smaller item from both halves
    while (i <= meio && j <= fim){
        if (cmp(&aux[i], &aux[j]) <= 0){
            v[k++] = aux[i++];
        }else{
            v[k++] = aux[j++];
        }
    }
    // copies remaining items from left half
    while (i <= meio){
        v[k++] = aux[i++];
    }
    // copies remaining items from right half
    while (j <= fim){
        v[k++] = aux[j++];
    }
}

// splits and sorts array halves
static void merge_sort_rec(EntradaHistorico *v, EntradaHistorico *aux,
                           int ini, int fim, FuncaoComparar cmp){
    if (ini >= fim){
        return;
    }
    int meio = ini + (fim - ini) / 2;
    merge_sort_rec(v, aux, ini, meio, cmp);
    merge_sort_rec(v, aux, meio + 1, fim, cmp);
    intercalar(v, aux, ini, meio, fim, cmp);
}

// creates helper array and starts sorting
static void merge_sort(EntradaHistorico *v, int n, FuncaoComparar cmp){
    EntradaHistorico *aux = malloc((size_t) n * sizeof(EntradaHistorico));
    merge_sort_rec(v, aux, 0, n - 1, cmp);
    free(aux);
}

// creates a sorted copy of history entries and measures the sorting time
void ordenar_historico(CriterioOrdenacao criterio, const Historico *historico, Metricas *metricas,
                       EntradaHistorico **copia_out, int *n_out, double *tempo_out){
    int n = historico->total;

    // makes space and copies original entries
    EntradaHistorico *copia = malloc((size_t) n * sizeof(EntradaHistorico));
    memcpy(copia, historico->entradas, (size_t) n * sizeof(EntradaHistorico));

    // picks comparison rule based on sort type
    FuncaoComparar cmp = comparar_pontuacao;
    if (criterio.tipo == ORDENAR_POR_NOME){
        cmp = comparar_nome;
    }
    Timer t;
    timer_iniciar(&t); // starts sorting timer
    merge_sort(copia, n, cmp);
    double segundos = timer_parar(&t);

    // updates sorting metrics and writes them to the log file
    metricas->ordenacao.ultimo_tempo_geral_s = segundos;
    metricas->ordenacao.ultimo_n_geral = n;
    gravar_relatorio_log(CFG_ARQUIVO_LOG, metricas);

    // returns sorted copy count and elapsed time
    *copia_out = copia;
    *n_out = n;
    *tempo_out = segundos;
}

// frees memory of sorted copy
void liberar_copia_ordenada(EntradaHistorico *copia){
    free(copia);
}

// returns name of the sorting algorithm
const char* nome_algoritmo_ordenacao(void){
    return "Merge Sort";
}