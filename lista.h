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

void lista_inicializar(Lista *l);

void inserir_fim(Lista *l, char simbolo);

void lista_destruir(Lista *l);

#endif