#ifndef AVL_HPP
#define AVL_HPP

#include "tipos.hpp"
#include "metricas.hpp"

// avl tree that stores all the orders
// it keeps itself balanced, so a search takes about log2(n) steps

// one node of the tree
struct NoAVL {
    Pedido  pedido;   // the order (code and recipe)
    int     altura;   // height of this node (leaf = 1)
    NoAVL  *esq;      // left child (smaller codes)
    NoAVL  *dir;      // right child (bigger codes)
};

// the tree: its root and where to write the counters
struct ArvoreAVL {
    NoAVL          *raiz;
    MetricasArvore *met;
};

// starts an empty tree
void avl_inicializar(ArvoreAVL *a, MetricasArvore *met);

// inserts an order
// returns 1 if inserted, 0 if the code already exists, -1 if there is no memory
int avl_inserir(ArvoreAVL *a, const Pedido *p);

// searches an order by code
// returns a pointer to the order, or NULL if it does not exist
const Pedido* avl_buscar(ArvoreAVL *a, int codigo);

// returns the height of the tree (empty = 0, only root = 1)
int avl_altura(const ArvoreAVL *a);

// frees all the nodes
void avl_destruir(ArvoreAVL *a);

#endif
