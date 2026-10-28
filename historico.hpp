#ifndef HISTORICO_HPP
#define HISTORICO_HPP

#include "tipos.hpp"

// ajustar parametros conforme estruturamos da lista contigua
bool carregar_historico(const char *caminho_arquivo);
bool adicionar_historico(const char *nome, int pontuacao);
bool salvar_historico(const char *caminho_arquivo);
const EntradaHistorico* obter_entradas_historico(int *total_out);
void liberar_historico();

#endif