#include "hash.h"
#include <stdlib.h>

static int indice_do_codigo(const TabelaHash *t, int codigo) {
    return (int) ((unsigned int) codigo % (unsigned int) t->m);
}

void hash_inicializar(TabelaHash *t, int m, MetricasHash *met) {
    t->m = m;
    t->met = met;

    t->slots = calloc((size_t) m, sizeof(NoHash*));

    met->m = m;
    met->n = 0;
    met->hits = 0;
    met->misses = 0;
    met->colisoes = 0;
}

int hash_buscar(TabelaHash *t, int codigo, Pedido *pedido_out) {
    for (const NoHash *no = t->slots[indice_do_codigo(t, codigo)]; no != NULL; no = no->prox) {
        if (no->pedido.codigo == codigo) {
            *pedido_out = no->pedido;
            t->met->hits++;
            return 1;
        }
    }
    t->met->misses++;
    return 0;
}

void hash_inserir(TabelaHash *t, const Pedido *p) {
    int i = indice_do_codigo(t, p->codigo);

    NoHash *novo = malloc(sizeof(NoHash));
    novo->pedido = *p;

    if (t->slots[i] != NULL)
        t->met->colisoes++;

    novo->prox = t->slots[i];
    t->slots[i] = novo;
    t->met->n++;
}

void hash_destruir(TabelaHash *t) {
    for (int i = 0; i < t->m; i++) {
        NoHash *no = t->slots[i];
        while (no != NULL) {
            NoHash *prox = no->prox;
            free(no);
            no = prox;
        }
    }
    free(t->slots);
    t->slots = NULL;
    t->m = 0;
}