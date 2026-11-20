#ifndef PEDIDOS_H
#define PEDIDOS_H
#include "tipos.h"
#include "lista.h"


// loads orders from a file
int carregar_pedidos(const char *caminho_arquivo);

// finds an order by its code
int get_pedido(int codigo, Pedido *pedido_out);

// picks a random order code
int sortear_codigo_pedido(void);

// frees memory and resets order data
void liberar_pedidos(void);

// resets search metrics for a new match
void pedidos_iniciar_partida(void);

// converts a recipe string into a list
void receita_para_lista(const char *receita, Lista *lista_out);

// sets the seed for the random generator
void pedidos_definir_semente(unsigned int semente);

#endif