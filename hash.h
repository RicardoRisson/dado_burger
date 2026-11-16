#ifndef HASH_H
#define HASH_H

#include "tipos.h"
#include "metricas.h"

typedef struct NoHash {
    Pedido  pedido;
    struct NoHash *prox;
} NoHash;

typedef struct {
    NoHash       **slots;
    int            m;
    MetricasHash  *met;
} TabelaHash;

void hash_inicializar(TabelaHash *t, int m, MetricasHash *met);

int hash_buscar(TabelaHash *t, int codigo, Pedido *pedido_out);

void hash_inserir(TabelaHash *t, const Pedido *p);

void hash_destruir(TabelaHash *t);

#endif