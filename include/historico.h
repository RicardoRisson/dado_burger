#ifndef HISTORICO_H
#define HISTORICO_H

#include "tipos.h"
#include "metricas.h"

#define TAM_CAMINHO 1024

// holds the history entries, the list size and the path to the history file
typedef struct {
    EntradaHistorico *entradas;
    int               total;
    int               capacidade;
    char              caminho[TAM_CAMINHO];
} Historico;

void carregar_historico(const char *caminho_arquivo, Historico *h, Metricas *metricas);

void adicionar_historico(Historico *h, const char *nome, int pontuacao);

void liberar_historico(Historico *h);

#endif