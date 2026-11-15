#include "lista.h"
#include <stdlib.h>


void lista_inicializar(Lista *l) {
    l->ini = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

void inserir_fim(Lista *l, char simbolo) {
    NoLista *novo = malloc(sizeof(NoLista));

    novo->simbolo = simbolo;
    novo->prox = NULL;
    novo->ant = l->fim;

    if (l->fim != NULL)
        l->fim->prox = novo;
    else
        l->ini = novo;

    l->fim = novo;
    l->tamanho++;
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