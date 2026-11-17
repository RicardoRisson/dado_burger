#include "fila.h"
#include <stdlib.h>

// inicializa
void fila_inicializar(Fila *f) {
    f->frente = NULL;
    f->final = NULL;
    f->tamanho = 0;
}

// esta vazia se sim retorna 1
int fila_vazia(const Fila *f) {
    return f->frente == NULL;
}


// entrou cliente novo na fila
void fila_entrar(Fila *f, const Cliente *c) {
    NoFila *novo = malloc(sizeof(NoFila));

    novo->dado = *c;
    novo->prox = NULL;

    if (f->final != NULL){
        f->final->prox = novo;
    }else{
        f->frente = novo;
    }
    
    f->final = novo;
    f->tamanho++;
}

// cliente atendido
void fila_sair(Fila *f) {
    NoFila *removido = f->frente;
    f->frente = removido->prox;

    if (f->frente == NULL){
        f->final = NULL;
    }

    free(removido);
    f->tamanho--;
}

// retorna ponteiro pro cliente da frente
Cliente* fila_frente(Fila *f) {
    return &f->frente->dado;
}

// destroi a fila, usado quando acaba a partida
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