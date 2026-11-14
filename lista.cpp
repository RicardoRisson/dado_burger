#include "lista.hpp"
#include <stdlib.h>

// o professor nao usou tamanho mas eu resolvi deixar o tamanho inicializado para facilitar 

void lista_inicializar(Lista *l) {
    l->ini = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

int inserir_fim(Lista *l, char simbolo) {
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));
    if (novo == NULL){
        return 0;
    } 

    novo->simbolo = simbolo;
    novo->prox = NULL;
    novo->ant = l->fim;

    if (l->fim != NULL){
        l->fim->prox = novo;
    }else{
        l->ini = novo;
    }

    l->fim = novo;
    l->tamanho++;
    return 1;
}

void lista_destruir(Lista *l) {
    NoLista *aux = l->ini;
    while (aux != NULL) {
        NoLista *prox = aux->prox;
        free(aux);
        aux = prox;
    }
    l->ini = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}