#ifndef HISTORICO_H
#define HISTORICO_H

#include "tipos.h"

void carregar_historico(const char *caminho_arquivo);

void adicionar_historico(const char *nome, int pontuacao);

const EntradaHistorico* obter_entradas_historico(int *total_out);

void liberar_historico(void);

#endif