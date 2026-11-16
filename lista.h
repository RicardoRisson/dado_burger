#ifndef LISTA_H
#define LISTA_H

typedef struct NoLista {
    char simbolo;
    struct NoLista *prox;
    struct NoLista *ant;
} NoLista;

typedef struct {
    NoLista *ini;
    NoLista *fim;
    int tamanho;
} Lista;

void inicializar(Lista *l);

void inserirFim(Lista *l, char simbolo);

void destruir(Lista *l);

#endif