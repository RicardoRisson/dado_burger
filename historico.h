#ifndef HISTORICO_H
#define HISTORICO_H

#include "tipos.h"

// loads saved history entries from a file
void carregar_historico(const char *caminho_arquivo);

// adds a new score entry to history
void adicionar_historico(const char *nome, int pontuacao);

// gets all history entries and total count
const EntradaHistorico* obter_entradas_historico(int *total_out);

// frees history memory and resets state
void liberar_historico(void);

#endif