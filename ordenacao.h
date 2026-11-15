#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "tipos.h"


typedef enum {
    ORDENAR_POR_NOME = 0,
    ORDENAR_POR_PONTUACAO = 1
} CriterioOrdenacao;

void ordenar_historico(CriterioOrdenacao criterio, EntradaHistorico **copia_out,
                       int *n_out, double *tempo_out);

void liberar_copia_ordenada(EntradaHistorico *copia);

const char* nome_algoritmo_ordenacao(void);

#endif
