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

int inserir_inicio(Lista *l, char simbolo) {
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));
    if (novo == NULL){
        return 0;
    }

    novo->simbolo = simbolo;
    novo->ant = NULL;
    novo->prox = l->ini;

    if (l->ini != NULL){
        l->ini->ant = novo;
    }else{
        l->fim = novo;
    }

    l->ini = novo;
    l->tamanho++;

    return 1;
}

int lista_remover(Lista *l, char simbolo) {
    NoLista *aux = l->ini;

    while (aux != NULL && aux->simbolo != simbolo) {
        aux = aux->prox;
    }

    if (aux == NULL) {
        return 0;
    }

    if (aux->ant != NULL) {
        aux->ant->prox = aux->prox;
    } else {
        l->ini = aux->prox;
    }

    if (aux->prox != NULL) {
        aux->prox->ant = aux->ant;
    } else {
        l->fim = aux->ant;
    }

    free(aux);
    l->tamanho--;

    return 1;
}

int lista_buscar(const Lista *l, char simbolo) {
    NoLista *aux = l->ini;

    while (aux != NULL) {
        if (aux->simbolo == simbolo) {
            return 1;
        }

        aux = aux->prox;
    }

    return 0;
}

int lista_tamanho(const Lista *l) {
    return l->tamanho;
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