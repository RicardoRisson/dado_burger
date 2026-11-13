#include "hash.hpp"
#include <stdlib.h>

// the slot is the remainder of code divided by m
// we use unsigned so a negative code never gives a negative slot
static int indice_do_codigo(const TabelaHash *t, int codigo) {
    return (int) ((unsigned int) codigo % (unsigned int) t->m);
}

int hash_inicializar(TabelaHash *t, int m, MetricasHash *met) {
    t->slots = NULL;
    t->m = 0;
    t->met = met;

    if (m <= 0) {
        return 0;                    // m must be bigger than zero
    }

    // calloc fills the memory with zeros, so every slot starts empty
    t->slots = (NoHash**) calloc((size_t) m, sizeof(NoHash*));
    if (t->slots == NULL) {
        return 0;
    }
    t->m = m;

    // starts the counters from zero
    met->m = m;
    met->n = 0;
    met->hits = 0;
    met->misses = 0;
    met->colisoes = 0;
    return 1;
}

int hash_buscar(TabelaHash *t, int codigo, Pedido *pedido_out) {
    if (t->slots != NULL) {
        // walks through the chain of the slot
        for (const NoHash *no = t->slots[indice_do_codigo(t, codigo)]; no != NULL; no = no->prox) {
            if (no->pedido.codigo == codigo) {
                *pedido_out = no->pedido;    // gives a copy of the order
                t->met->hits++;
                return 1;
            }
        }
    }
    t->met->misses++;
    return 0;
}

int hash_inserir(TabelaHash *t, const Pedido *p) {
    if (t->slots == NULL) {
        return -1;
    }
    int i = indice_do_codigo(t, p->codigo);

    // does not save the same code twice
    for (const NoHash *no = t->slots[i]; no != NULL; no = no->prox) {
        if (no->pedido.codigo == p->codigo) {
            return 0;
        }
    }

    NoHash *novo = (NoHash*) malloc(sizeof(NoHash));
    if (novo == NULL) {
        return -1;
    }
    novo->pedido = *p;               // stores a copy of the order in the new entry

    // if the slot is already used it is a collision (counts one, whatever the chain size)
    if (t->slots[i] != NULL) {
        t->met->colisoes++;
    }

    novo->prox = t->slots[i];        // puts the new entry at the start of the chain
    t->slots[i] = novo;
    t->met->n++;
    return 1;
}

double hash_fator_de_carga(const TabelaHash *t) {
    if (t->m == 0) {
        return 0.0;
    }
    return (double) t->met->n / (double) t->m;
}

void hash_destruir(TabelaHash *t) {
    if (t->slots != NULL) {
        for (int i = 0; i < t->m; i++) {
            NoHash *no = t->slots[i];
            while (no != NULL) {
                NoHash *prox = no->prox;     // saves the next one before freeing
                free(no);
                no = prox;
            }
        }
        free(t->slots);
    }
    t->slots = NULL;
    t->m = 0;
}
