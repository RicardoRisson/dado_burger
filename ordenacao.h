#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "tipos.h"

#define ORDENAR_POR_NOME      0
#define ORDENAR_POR_PONTUACAO 1

typedef struct {
    int tipo;
} CriterioOrdenacao;

void ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out);

void liberar_copia_ordenada(EntradaHistorico *copia);

const char* nome_algoritmo_ordenacao(void);

#endif