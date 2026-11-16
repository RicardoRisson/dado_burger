#ifndef PEDIDOS_H
#define PEDIDOS_H
#include "tipos.h"
#include "lista.h"


int carregar_pedidos(const char *caminho_arquivo);

int get_pedido(int codigo, Pedido *pedido_out);

int sortear_codigo_pedido(void);

void liberar_pedidos(void);

void pedidos_iniciar_partida(void);

void receita_para_lista(const char *receita, Lista *lista_out);

void pedidos_definir_semente(unsigned int semente);

#endif