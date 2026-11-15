#ifndef AVL_H
#define AVL_H

#include "tipos.h"
#include "metricas.h"


typedef struct NoAVL {
    Pedido  pedido;
    int     altura;
    struct NoAVL *esq;
    struct NoAVL *dir;
} NoAVL;

typedef struct {
    NoAVL          *raiz;
    MetricasArvore *met;
} ArvoreAVL;

void avl_inicializar(ArvoreAVL *a, MetricasArvore *met);

int avl_inserir(ArvoreAVL *a, const Pedido *p);

const Pedido* avl_buscar(ArvoreAVL *a, int codigo);

int avl_altura(const ArvoreAVL *a);

void avl_destruir(ArvoreAVL *a);

#endif
