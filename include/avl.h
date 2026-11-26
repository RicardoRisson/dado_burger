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

void avl_inicializar(ArvoreAVL *a, MetricasArvore *met);

int avl_inserir(ArvoreAVL *a, const Pedido *p);

const Pedido* avl_buscar(ArvoreAVL *a, int codigo);

int avl_codigo_menor(const ArvoreAVL *a);
int avl_codigo_maior(const ArvoreAVL *a);

int avl_altura(const ArvoreAVL *a);

void avl_destruir(ArvoreAVL *a);

#endif