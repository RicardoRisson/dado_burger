#ifndef PEDIDOS_H
#define PEDIDOS_H
#include "tipos.h"
#include "lista.h"
#include "avl.h"
#include "splay.h"
#include "hash.h"
#include "metricas.h"


int carregar_pedidos(const char *caminho_arquivo, ArvoreAVL *avl, ArvoreSplay *splay, Metricas *metricas);

int get_pedido(int codigo, Pedido *pedido_out, ArvoreAVL *avl, ArvoreSplay *splay, const Metricas *metricas);

void sortear_codigos_pedidos(int *vetor, int quantidade, int inf, int sup, TabelaHash *hash, Metricas *metricas);

void liberar_pedidos(ArvoreAVL *avl, ArvoreSplay *splay);

void pedidos_iniciar_partida(TabelaHash *hash, Metricas *metricas);

void receita_para_lista(const char *receita, Lista *lista_out);

#endif