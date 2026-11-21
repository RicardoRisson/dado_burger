#ifndef SPLAY_H
#define SPLAY_H

#include "tipos.h"
#include "metricas.h"


// represents a node in the splay tree
typedef struct NoSplay {
    Pedido    pedido;
    struct NoSplay  *esq;
    struct NoSplay  *dir;
} NoSplay;

// holds the splay tree root and metrics
typedef struct {
    NoSplay        *raiz;
    MetricasArvore *met;
} ArvoreSplay;

// sets up an empty splay tree
void splay_inicializar(ArvoreSplay *a, MetricasArvore *met);

// inserts a new order into the tree
int splay_inserir(ArvoreSplay *a, const Pedido *p);

// finds an order by its code
const Pedido* splay_buscar(ArvoreSplay *a, int codigo);

// calculates and returns the tree height
int splay_calcular_altura(ArvoreSplay *a);

// frees all nodes and resets the tree
void splay_destruir(ArvoreSplay *a);

#endif