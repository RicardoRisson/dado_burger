#ifndef SPLAY_H
#define SPLAY_H

#include "tipos.h"
#include "metricas.h"


typedef struct NoSplay {
    Pedido    pedido;
    struct NoSplay  *esq;
    struct NoSplay  *dir;
} NoSplay;

typedef struct {
    NoSplay        *raiz;
    MetricasArvore *met;
} ArvoreSplay;

void splay_inicializar(ArvoreSplay *a, MetricasArvore *met);

int splay_inserir(ArvoreSplay *a, const Pedido *p);

const Pedido* splay_buscar(ArvoreSplay *a, int codigo);

int splay_calcular_altura(ArvoreSplay *a);

void splay_destruir(ArvoreSplay *a);

#endif
