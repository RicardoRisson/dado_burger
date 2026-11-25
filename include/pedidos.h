#ifndef PEDIDOS_H
#define PEDIDOS_H
#include "tipos.h"
#include "lista.h"
#include "avl.h"
#include "splay.h"
#include "hash.h"
#include "metricas.h"


// loads orders from a file into the avl and splay trees
int carregar_pedidos(const char *caminho_arquivo, ArvoreAVL *avl, ArvoreSplay *splay, Metricas *metricas);

// finds an order by its code
int get_pedido(int codigo, Pedido *pedido_out, ArvoreAVL *avl, ArvoreSplay *splay, const Metricas *metricas);

// draws quantidade different order codes between inf and sup, using the hash to detect repeated ones
void sortear_codigos_pedidos(int *vetor, int quantidade, int inf, int sup, TabelaHash *hash, Metricas *metricas);

// frees the trees
void liberar_pedidos(ArvoreAVL *avl, ArvoreSplay *splay);

// resets search metrics and the hash for a new match
void pedidos_iniciar_partida(TabelaHash *hash, Metricas *metricas);

// converts a recipe string into a list
void receita_para_lista(const char *receita, Lista *lista_out);

#endif