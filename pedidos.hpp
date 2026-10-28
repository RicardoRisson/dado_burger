#ifndef PEDIDOS_HPP
#define PEDIDOS_HPP

#include "tipos.hpp"

// ajustar parametros conforme a gente for implementando AVL e Hash 
bool carregar_pedidos(const char *caminho_arquivo);
bool get_pedido(int codigo, Pedido *pedido_out);
int sortear_codigo_pedido();
void liberar_pedidos();

#endif