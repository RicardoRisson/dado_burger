#include "lista.h"
#include <stdlib.h>

// cria a lista vazia 
void inicializar(Lista *l) {
    l->ini = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

// coloca um ingrediente no final da lista
void inserirFim(Lista *l, char simbolo) {
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));

    novo->simbolo = simbolo;
    novo->prox = NULL;
    novo->ant = l->fim;

    // se a lista ja tinha no o antigo ultimo aponta pro novo senao o novo e o primeiro
    if (l->fim != NULL)
        l->fim->prox = novo;
    else
        l->ini = novo;

    l->fim = novo;
    l->tamanho++;
}

// quando a receita do cliente n é mais necessaria libera todos os nos
void destruir(Lista *l) {
    NoLista *aux = l->ini;
    while (aux != NULL) {
        // guarda o proximo antes de dar free p nao perder o resto
        NoLista *prox = aux->prox;
        free(aux);
        aux = prox;
    }
    l->ini = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}