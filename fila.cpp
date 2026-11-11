
#include "fila.hpp"
#include <stdlib.h>

void fila_inicializar(Fila *f) {
    f->frente = NULL;
    f->final = NULL;
    f->tamanho = 0;
}

int fila_vazia(const Fila *f) {
    return f->frente == NULL;
}

int fila_entrar(Fila *f, const Cliente *c) {
    NoFila *novo = (NoFila*) malloc(sizeof(NoFila));

    if (novo == NULL) {
        return 0;
    }

    novo->dado = *c;
    novo->prox = NULL;

    if (f->final != NULL) {
        f->final->prox = novo;
    } else {
        f->frente = novo;
    }

    f->final = novo;
    f->tamanho++;

    return 1;
}

int fila_sair(Fila *f) {
    NoFila *removido;

    if (fila_vazia(f)) {
        return 0;
    }

    removido = f->frente;
    f->frente = removido->prox;

    if (f->frente == NULL) {
        f->final = NULL;
    }

    free(removido);
    f->tamanho--;

    return 1;
}

Cliente* fila_frente(Fila *f) {
    if (fila_vazia(f)) {
        return NULL;
    }

    return &f->frente->dado;
}

int fila_tamanho(const Fila *f) {
    return f->tamanho;
}

void fila_destruir(Fila *f) {
    NoFila *aux;

    while (f->frente != NULL) {
        aux = f->frente;
        f->frente = aux->prox;
        free(aux);
    }

    f->final = NULL;
    f->tamanho = 0;
}