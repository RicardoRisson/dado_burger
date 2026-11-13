#ifndef SPLAY_HPP
#define SPLAY_HPP

#include "tipos.hpp"
#include "metricas.hpp"

// splay tree (extra of the project), it has the same functions as the avl
// every time a node is used, the tree moves it to the root
// so the nodes used often stay near the top

// one node of the tree (no height field, a splay does not need it)
struct NoSplay {
    Pedido    pedido;
    NoSplay  *esq;
    NoSplay  *dir;
};

// the tree: its root and where to write the counters
struct ArvoreSplay {
    NoSplay        *raiz;
    MetricasArvore *met;
};

// starts an empty tree
void splay_inicializar(ArvoreSplay *a, MetricasArvore *met);

// inserts an order, the new node becomes the root
// returns 1 if inserted, 0 if the code already exists, -1 if there is no memory
// it does not update the height, call splay_calcular_altura after loading
int splay_inserir(ArvoreSplay *a, const Pedido *p);

// searches an order by code and moves the node found to the root
// if the code does not exist, the last node visited goes to the root
// returns a pointer to the order, or NULL if it does not exist
const Pedido* splay_buscar(ArvoreSplay *a, int codigo);

// measures the height (empty = 0, only root = 1), saves it and returns it
// it uses a loop and not recursion because the tree can become a long line
// returns -1 if there is no memory
int splay_calcular_altura(ArvoreSplay *a);

// frees all the nodes (also without recursion)
void splay_destruir(ArvoreSplay *a);

#endif
