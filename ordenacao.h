#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "tipos.h"

// sorting option constants
#define ORDENAR_POR_NOME      0
#define ORDENAR_POR_PONTUACAO 1

// holds the chosen sorting option
typedef struct {
    int tipo;
} CriterioOrdenacao;

// returns a sorted copy of history entries
void ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out);

// frees memory used by the sorted copy
void liberar_copia_ordenada(EntradaHistorico *copia);

// returns the name of the sorting algorithm
const char* nome_algoritmo_ordenacao(void);

#endif