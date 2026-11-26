#ifndef AVL_H
#define AVL_H

#include "tipos.h"
#include "metricas.h"

// avl tree: stores all the orders and keeps itself balanced

typedef struct NoAVL {
    Pedido pedido;        // the order
    int    altura;        // height of this node (leaf = 0, NULL = -1)
    struct NoAVL *esq;    // left child, smaller codes
    struct NoAVL *dir;    // right child, bigger codes
} NoAVL;

typedef struct {
    NoAVL          *raiz;
    MetricasArvore *met;  // where the counters are written
} ArvoreAVL;

// starts an empty tree
void avl_inicializar(ArvoreAVL *a, MetricasArvore *met);

// inserts an order, returns 1 = inserted, 0 = code already exists, -1 = no memory
int avl_inserir(ArvoreAVL *a, const Pedido *p);

// searches by code, returns a pointer to the order or NULL
const Pedido* avl_buscar(ArvoreAVL *a, int codigo);

// returns the smallest and the biggest order code in the tree (empty = 0)
int avl_codigo_menor(const ArvoreAVL *a);
int avl_codigo_maior(const ArvoreAVL *a);

// returns the height in levels (empty = 0, only root = 1)
int avl_altura(const ArvoreAVL *a);

// frees all the nodes
void avl_destruir(ArvoreAVL *a);

#endif